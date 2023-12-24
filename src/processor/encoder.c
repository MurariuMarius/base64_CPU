#include <stdint.h>
#include <stdio.h>

#include "uprogram_sequencer.h"

#define uPROGRAM_LENGTH 57

extern signal ZF;

/**
 * Control field signals:
 * http://tinyurl.com/4w52dawb
*/
uInstruction controlMemory[uPROGRAM_LENGTH] = {

// PREPARE: 0                  432109876543210
    (uint3_t){0}, (uint15_t){0b000000000011100},  // LDR Y, 360
    (uint3_t){0}, (uint15_t){0b000000000101101},  // PSH X
    (uint3_t){0}, (uint15_t){0b000000001001100},  // MOV X, #0
    (uint3_t){0}, (uint15_t){0b000000001001100},  // MOV Y, #0
    (uint3_t){0}, (uint15_t){0b000010000000000},  // reset
// BEGIN: 5                    432109876543210
    (uint3_t){0}, (uint15_t){0b001000000000000},  // LDR R_IN, [X]
// SCAN: 6
    (uint3_t){0}, (uint15_t){0b000000000000000},  // if state = 0 go to S0 - TODO
    (uint3_t){0}, (uint15_t){0b000000000000000},  // if state = 1 go to S1 - TODO
    (uint3_t){0}, (uint15_t){0b001010000000000},  // BUFF3[3:0] = R_IN[15:12]
    (uint3_t){0}, (uint15_t){0b100010000000000}   // endProc
};

static uint10_t instructionCodeMemory[uPROGRAM_LENGTH] = {
    {0b1101101000}, // LDR Y, 360
    {0b1000000000}, // PSH X
    {0b0000000000}, // MOV X, #0
    {0b1000000000}, // MOV Y, #0
    {0}, // reset
    {0b0000000000}, // LDR R_IN, [X]
    {0}, // if state = 0
    {0}, // if state = 1
    {0}, // BUFF3[3:0] = R_IN[15:12]
    {0}, // endProc
};

static uint6_t opcodeMemory[uPROGRAM_LENGTH];

static uint16_t R_IN;
static uint16_t R_OUT;

static uint6_t buff_1;
static uint6_t buff_2;
static uint6_t buff_3;

static uint2_t selReg;
static uint2_t state;

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

void reset() {
    selReg.val = 0;
    buff_1.val = 0;
    buff_2.val = 0;
    state.val = 0;
}

void reset_Buff_3() {
    buff_3.val = 0;
}

void reset_R_OUT() {
    R_OUT = 0;
}

void load_R_IN() {
    R_IN = load(getIndex());
}

void ld30() {
    buff_3.val = ((R_IN >> 12) << 2) | (buff_3.val & 0x3);
}

void (*signalActions[])() = {
    &nop,
    &reset,
    &reset_Buff_3,
    &reset_R_OUT,
    &load_R_IN,
    &ld30,
    &nop,
    &nop,
    &nop,
    &nop,
    &nop,
    &nop,
    &nop,
    &nop,
    &nop,
    &activateLoad_uPC,
    &activateRestore_uPC,
    &activateEndProc
};

void encode() {
    loadMicroprogram(controlMemory, instructionCodeMemory, opcodeMemory, &loadAddress, signalActions);
}
