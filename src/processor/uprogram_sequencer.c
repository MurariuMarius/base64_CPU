#include <stdint.h>
#include <stdio.h>

#include "uprogram_sequencer.h"

static uint8_t getBranchAddress(uInstruction instruction);
static void activateExternalControlSignals();
static void activateControlSignals() ;
static void next_uPC();
static void decode();
static void executeMicroprogram();
static void resetBranchingSignals();

static uint8_t uPC;
static uint8_t SBR; // subroutine register

static signal endProc;
static signal restore_uPC;
static signal load_uPC;

static signal *externalContorlSignals[EXTERNAL_CS_COUNT] = {
    (signal *)&stackOP, 
    NULL,   // stackOP is uint2_t
    &enc,
    &immOp,
    &ldm,
    &str,
    &lse,
    NULL,
};

uint3_t conditionSelect;
uint8_t branchAddress;
uint25_t controlField;

static uInstruction *controlMemory;
static uint10_t *instructionCodeMemory;
static uint6_t *opcodeMemory;

static signal (*loadAddress)();
static void (**signalActions)();

void loadMicroprogram(uInstruction *cm, uint10_t *icm, uint6_t *opm, signal (*la)(), void (*sa[])()) {
    controlMemory = cm;
    instructionCodeMemory = icm;
    opcodeMemory = opm;
    loadAddress = la;
    signalActions = sa;

    executeMicroprogram();
}

void fetchNextInstruction() {
    uInstruction instruction = controlMemory[uPC];
    printf("\tMicroinstruction: %d %x %x\n", uPC, instruction.conditionSelect.val, instruction.BA_CF.val);
    conditionSelect = instruction.conditionSelect;
    printf("ENC: C/S: %d\n", conditionSelect.val);
    if (conditionSelect.val != 0) {
        branchAddress = getBranchAddress(instruction);
    } else {
        controlField = instruction.BA_CF;
        activateControlSignals();
    }
}

uint10_t getInstruction() {
    return instructionCodeMemory[uPC];
}

static void executeMicroprogram() {
    uPC = 0;
    while (!endProc.active) {
        resetBranchingSignals();
        fetchNextInstruction();
        next_uPC();
    }
}

static void activateControlSignals() {
    activateExternalControlSignals();
    decode();
}

static void activateExternalControlSignals() {
    uint8_t i = 0;

    stackOP.val = (controlField.val & (1 << i)) != 0;
    i++;
    stackOP.val |= ((controlField.val & (1 << i)) != 0) << 1;
    
    for (; i < EXTERNAL_CS_COUNT; i++) {
        if (!externalContorlSignals[i]) continue;

        *(externalContorlSignals[i]) = (controlField.val & (1 << i)) ? ACTIVE : INACTIVE;
    }

    if (enc.active) {
        register_file();
    }
}

static void next_uPC() {
    if (loadAddress().active) {
        uPC = branchAddress;
    } else {
        if (restore_uPC.active) {
            uPC = SBR;
        }
        uPC++;
    }

    if (load_uPC.active) {
        SBR = uPC;
        printf("uPS: SBR: %d\n", SBR);
    }
    printf("uPS: uPC: %d\n", uPC);
}

static uint8_t getBranchAddress(uInstruction instruction) {
    uint8_t mask = (1 << BA_SIZE) - 1;
    uint8_t branchAddress = (uint8_t)(instruction.BA_CF.val & mask);
    printf("ENC: BA: %d\n", branchAddress);

    return branchAddress;
}

static void resetBranchingSignals() {
    load_uPC = INACTIVE;
    restore_uPC = INACTIVE;
}

void activateEndProc() {
    endProc = ACTIVE;
}

void activateLoad_uPC() {
    load_uPC = ACTIVE;
}

void activateRestore_uPC() {
    restore_uPC = ACTIVE;
}

static void decode() {
    if (controlField.val & 0x1000000) {
        signalActions[0]();
    }
    if (controlField.val & 0x800000) {
        signalActions[1]();
    }
    if (controlField.val & 0x400000) {
        signalActions[2]();
    }
}
