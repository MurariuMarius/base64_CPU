#include <stdint.h>
#include <stdio.h>

#include "uprogram_sequencer.h"

extern signal ZF;

/**
 * Control field signals:
 * http://tinyurl.com/4w52dawb
*/
uInstruction controlMemory[uPROGRAM_LENGTH] = {

//                             4321098765432109876543210
    (uint3_t){0}, (uint25_t){0b0000000000000000000011100},  // LDR Y, 360
    (uint3_t){7}, (uint25_t){0b0000000000000000000001000},  // branch_always
    (uint3_t){0}, (uint25_t){0b0000000000000000000101101},  // PSH X
    (uint3_t){0}, (uint25_t){0b0000000000000000001001100},  // MOV X, #0
    (uint3_t){0}, (uint25_t){0b0000000000000000001001100},  // MOV Y, #0
    (uint3_t){0}, (uint25_t){0b0000000000000000000010100},  // LDR Y, [SP+1]
    (uint3_t){0}, (uint25_t){0b0010000000000000000000000},  // ret
    (uint3_t){0}, (uint25_t){0b0000000000000000001001100},  // MOV Y, #0
    (uint3_t){0}, (uint25_t){0b0100000000000000000000000},  // sbr
    (uint3_t){7}, (uint25_t){0b0000000000000000000000010},  // jmp
    (uint3_t){0}, (uint25_t){0b1000000000000000000000000}   // endProc
};

static uint10_t instructionCodeMemory[uPROGRAM_LENGTH] = {
    {0b1101101000}, // LDR Y, 360
    {0},
    {0b1000000000}, // PSH X
    {0b0000000000}, // MOV X, #0
    {0b1000000000}, // MOV Y, #0
    {0b1110000001}, // LDR Y, [SP+1]
    {0}, // ret
    {0b1000000000}, // MOV Y, #0
    {0}, // sbr
    {0}, // jmp
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


void (*signalActions[])() = {
    &activateEndProc,
    &activateLoad_uPC,
    &activateRestore_uPC
};

void encode() {
    loadMicroprogram(controlMemory, instructionCodeMemory, opcodeMemory, &loadAddress, signalActions);
}
