#include <stdint.h>
#include <stdio.h>

#include "internals.h"


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

static uint8_t uPC;

static signal *controlSignals[] = {
    &ldm,
    &immOp,
    &enc
};

static uint3_t conditionSelect;
static uint8_t branchAddress;
static uint25_t controlField;

uInstruction controlMemory[] = {
    (uint3_t){0}, (uint25_t){0b0000000000000000000000111}
};

static uint10_t instructionCodeMemory[uPROGRAM_LENGTH] = {
    (uint10_t){0b1101101000}
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
    conditionSelect = instruction.conditionSelect;
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

static void activateControlSignals() {
    printf("ENC %d\n", enc.active);
    for (uint8_t i = 0 ; i < CF_SIZE; i++) {
        if (i > 2) continue;
        *(controlSignals[i]) = (controlField.val & (1 << i)) ? ACTIVE : INACTIVE;
    }
    printf("ENC %d\n", enc.active);
    register_file();
}

void nextInstruction() {
    uPC++;
}


static uint8_t getBranchAddress(uInstruction instruction) {
    uint8_t mask = (1 << BA_SIZE) - 1;
    uint8_t branchAddress = (uint8_t)(instruction.BA_CF.val & mask);

    return branchAddress;
}