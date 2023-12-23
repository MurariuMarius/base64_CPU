#include <stdint.h>
#include <stdio.h>

#include "internals.h"

extern signal ZF;

#define uPROGRAM_LENGTH 55
#define BA_SIZE 8
#define CF_SIZE 25

typedef struct {
    unsigned int val : 3;
} uint3_t;

typedef struct {
    unsigned int val : CF_SIZE;
} uint25_t;

typedef struct {
    uint3_t conditionSelect;
    uint25_t BA_CF;
} uInstruction;

static uint8_t getBranchAddress(uInstruction instruction);
static void activateControlSignals();
static void next_uPC();
static signal loadAddress();

static uint8_t uPC;

static signal endProc;

static signal *controlSignals[CF_SIZE] = {
    (signal *)&stackOP, 
    NULL,   // stackOP is uint2_t
    &enc,
    &immOp,
    &ldm,
    &str,
    &lse,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    &endProc
};

static uint3_t conditionSelect;
static uint8_t branchAddress;
static uint25_t controlField;

/**
 * Control field signals:
 * http://tinyurl.com/4w52dawb
*/
uInstruction controlMemory[uPROGRAM_LENGTH] = {

//                             4321098765432109876543210
    (uint3_t){0}, (uint25_t){0b0000000000000000000011100},  // LDR Y, 360
    (uint3_t){0}, (uint25_t){0b0000000000000000000101101},  // PSH X
    (uint3_t){0}, (uint25_t){0b0000000000000000001001100},  // MOV X, #0
    (uint3_t){0}, (uint25_t){0b0000000000000000001001100},  // MOV Y, #0
    (uint3_t){0}, (uint25_t){0b0000000000000000000010100},  // LDR Y, [SP+1]
    (uint3_t){0}, (uint25_t){0b1000000000000000000000000}   // endProc
};

static uint10_t instructionCodeMemory[uPROGRAM_LENGTH] = {
    {0b1101101000}, // LDR Y, 360
    {0b1000000000}, // PSH X
    {0b0000000000}, // MOV X, #0
    {0b1000000000}, // MOV Y, #0
    {0b1110000001}, // LDR Y, [SP+1]
    {0}
};

static uint6_t opcodeMemory[uPROGRAM_LENGTH];

static uint16_t R_IN;
static uint16_t R_OUT;

static uint6_t buff_1;
static uint6_t buff_2;
static uint6_t buff_3;

static uint2_t selReg;
static uint2_t state;

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

void executeMicroprogram() {
    uPC = 0;
    while (!endProc.active) {
        fetchNextInstruction();
        next_uPC();
    }
}

static void activateControlSignals() {

    uint8_t i = 0;

    stackOP.val = (controlField.val & (1 << i)) != 0;
    i++;
    stackOP.val |= ((controlField.val & (1 << i)) != 0) << 1;
    
    for (i; i < CF_SIZE; i++) {
        if (!controlSignals[i]) continue;

        *(controlSignals[i]) = (controlField.val & (1 << i)) ? ACTIVE : INACTIVE;
    }

    if (enc.active) {
        register_file();
    }
}

static void next_uPC() {
    if (loadAddress().active) {
        uPC = branchAddress;
    } else {
        uPC++;
    }
}

static signal loadAddress() {
    switch(conditionSelect.val) {
        case 0b000:
            return INACTIVE;
        case 0b001:
            return (signal){state.val == 0};
        case 0b010:
            return (signal){state.val == 1};
        case 0b011:
            return (signal){state.val == 2};
        case 0b100:
            return ZF;
        case 0b101:
            return ((selReg.val & 1) | ZF.active) ? ACTIVE : INACTIVE;
        case 0b110:
            return selReg.val == 2 ? ACTIVE : INACTIVE;
        case 0b111:
            return ACTIVE;
    }
    return INACTIVE;
}

static uint8_t getBranchAddress(uInstruction instruction) {
    uint8_t mask = (1 << BA_SIZE) - 1;
    uint8_t branchAddress = (uint8_t)(instruction.BA_CF.val & mask);
    printf("ENC: BA: %d\n", branchAddress);

    return branchAddress;
}
