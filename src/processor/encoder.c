#include <stdint.h>
#include <stdio.h>

#include "opcodes.h"
#include "uprogram_sequencer.h"

/*   INTERNAL CONTROL SIGNALS   */
#define SIG_RESET           0b00001
#define SIG_RESET_BUFF_3    0b00010
#define SIG_RESET_R_OUT     0b00011
#define SIG_LD_R_IN         0b00100
#define SIG_LD30            0b00101
#define SIG_LSH4            0b00110
#define SIG_LD52            0b00111
#define SIG_LD10            0b01000
#define SIG_LSH2            0b01001
#define SIG_LD54            0b01010
#define SIG_LD_BUFF         0b01011
#define SIG_INC_STATE       0b01100
#define SIG_LD_ROM          0b01101
#define SIG_INC_SEL_REG     0b01110
#define SIG_RESET_LD_ROM    0b01111
#define SIG_STR_R_OUT       0b10000
#define SIG_LOAD_uPC        0b10001
#define SIG_RESTORE_uPC     0b10010
#define SIG_END_PROC        0b10011

/*  LABELS  */
#define BEGIN 6
#define S0 15
#define S1 17
#define PUSH 23
#define INC_STATE 31
#define END 35
#define EXIT 38
#define PROC_OUTBUS 40
#define STORE 43
#define INC_SEL_REG 46

#define uPROGRAM_LENGTH 57

#define generateCF(externalControlSignals, internalControlSignals) \
(uint15_t){(0x3FF & (externalControlSignals)) | ((internalControlSignals) << EXTERNAL_CS_COUNT)}

#define generateBA(address) (uint15_t){address}

extern signal ZF;

/**
 * Control field signals:
 * http://tinyurl.com/4w52dawb
*/
uInstruction controlMemory[uPROGRAM_LENGTH] = {

// PREPARE: 0
    (uint3_t){0}, generateCF(SIG_ENC | SIG_IMM_OP | SIG_LDM, SIG_NOP),                         // LDR Y, 360
    (uint3_t){0}, generateCF(SIG_STACK_OP_PSH | SIG_ENC | SIG_IMM_OP | SIG_STR, SIG_NOP),      // PSH X    
    (uint3_t){0}, generateCF(SIG_ENC | SIG_IMM_OP | SIG_LSE, SIG_NOP),                         // MOV X, #0
    (uint3_t){0}, generateCF(SIG_ENC | SIG_IMM_OP | SIG_LSE, SIG_RESET),                       // MOV Y, #0, reset
    (uint3_t){0}, generateCF(SIG_NOP, SIG_RESET_BUFF_3),                                       // reset_Buff_3
    (uint3_t){0}, generateCF(SIG_NOP, SIG_RESET_R_OUT),                                        // reset_R_OUT
// BEGIN: 6                    
    (uint3_t){0}, generateCF(SIG_ENC, SIG_LD_R_IN),                                            // LDR R_IN, [X]
// SCAN: 7
    (uint3_t){1}, generateBA(S0),                                                              // if state = 0 go to S0
    (uint3_t){2}, generateBA(S1),                                                              // if state = 1 go to S1
    (uint3_t){0}, generateCF(SIG_NOP, SIG_LD30),                                               // BUFF3[3:0] = R_IN[15:12]
    (uint3_t){0}, generateCF(SIG_NOP, SIG_LSH4),                                               // lsh4
    (uint3_t){0}, generateCF(SIG_NOP, SIG_LOAD_uPC),                                           // load_uPC
    (uint3_t){7}, generateBA(PROC_OUTBUS),                                                     // jmp outbus
    (uint3_t){7}, generateBA(PUSH),                                                            // go to PUSH
// S0: 14                      
    (uint3_t){0}, generateCF(SIG_NOP, SIG_RESET_BUFF_3),                                       // reset_Buff_3
    (uint3_t){0}, generateCF(SIG_NOP, SIG_LD52),                                               // ld52
    (uint3_t){7}, generateBA(PUSH),                                                            // go to PUSH
// S1: 17                      
    (uint3_t){0}, generateCF(SIG_NOP, SIG_LD10),                                               // ld10
    (uint3_t){0}, generateCF(SIG_NOP, SIG_LSH2),                                               // lsh2
    (uint3_t){0}, generateCF(SIG_NOP, SIG_LOAD_uPC),                                           // load_uPC
    (uint3_t){7}, generateBA(PROC_OUTBUS),                                                     // jmp outbus
    (uint3_t){0}, generateCF(SIG_NOP, SIG_RESET_BUFF_3),                                       // reset_Buff_3
    (uint3_t){0}, generateCF(SIG_NOP, SIG_LD54),                                               // ld54
// PSH: 23
    (uint3_t){0}, generateCF(SIG_NOP, SIG_LD_BUFF),                                            // loadBuffers
// OUTPUT: 24
    (uint3_t){0}, generateCF(SIG_NOP, SIG_LOAD_uPC),                                           // load_uPC
    (uint3_t){7}, generateBA(PROC_OUTBUS),                                                     // jmp outbus
    (uint3_t){0}, generateCF(SIG_NOP, SIG_LOAD_uPC),                                           // load_uPC
    (uint3_t){7}, generateBA(PROC_OUTBUS),                                                     // jmp outbus
// NEXT: 28
    (uint3_t){0}, generateCF(SIG_ENC | SIG_IMM_OP | SIG_ALU_OP | SIG_ENC_INSTR, SIG_NOP),      // INC X
    (uint3_t){0}, generateCF(SIG_ENC | SIG_LDM, SIG_NOP),                                      // LDR Y, [SP+1]
    (uint3_t){0}, generateCF(SIG_ENC | SIG_ALU_OP | SIG_ENC_INSTR, SIG_NOP),                   // CMP X, Y
// INC_STATE: 31
    (uint3_t){0}, generateCF(SIG_NOP, SIG_INC_STATE),                                          // state++  
    (uint3_t){3}, generateBA(INC_STATE),                                                       // if state = 3 go to INC_STATE 
    (uint3_t){4}, generateBA(END),                                                             // if ZF = 1 go to END
    (uint3_t){7}, generateBA(BEGIN),                                                           // go to BEGIN
// END: 35
    (uint3_t){1}, generateBA(EXIT),                                                            // if STATE = 0 go to EXIT
    (uint3_t){0}, generateCF(SIG_NOP, SIG_LOAD_uPC),                                           // load_uPC
    (uint3_t){7}, generateBA(PROC_OUTBUS),                                                     // jmp outbus    
// EXIT: 38
    (uint3_t){0}, generateCF(SIG_STACK_OP_POP | SIG_ENC | SIG_IMM_OP | SIG_LDM, SIG_NOP),      // POP Y
    (uint3_t){0}, generateCF(SIG_NOP, SIG_END_PROC),                                           // endProc
// PROC OUTBUS: 40
    (uint3_t){0}, generateCF(SIG_NOP, SIG_LD_ROM),                                             // ld_rom = 1
    (uint3_t){5}, generateBA(STORE),                                                           // if selReg[0] or ZF go to STORE
    (uint3_t){7}, generateBA(INC_SEL_REG),                                                     // go to INC
// STORE: 43
    (uint3_t){0}, generateCF(SIG_ENC | SIG_STR, SIG_STR_R_OUT),                                // STR R_OUT, [Y]
    (uint3_t){0}, generateCF(SIG_NOP, SIG_RESET_R_OUT)    ,                                    // R_OUT = 0 
    (uint3_t){0}, generateCF(SIG_ENC | SIG_IMM_OP | SIG_ALU_OP | SIG_ENC_INSTR, SIG_NOP),      // INC Y
// INC_SEL_REG: 46
    (uint3_t){0}, generateCF(SIG_NOP, SIG_INC_SEL_REG),                                        // selReg++
    (uint3_t){6}, generateBA(INC_SEL_REG),                                                     // if selReg = 3 go to INC_SEL_REG
    (uint3_t){0}, generateCF(SIG_NOP, SIG_RESET_LD_ROM),                                       // ld_rom = 0
    (uint3_t){0}, generateCF(SIG_NOP, SIG_RESTORE_uPC)                                         // restore_uPC
};

static uint10_t instructionCodeMemory[uPROGRAM_LENGTH] = {
    {0b1101101000}, // LDR Y, 360
    {0b0000000000}, // PSH X
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
static signal ld_rom;

static signal loadAddress() {
    switch(conditionSelect.val) {
        case 0b000:
            return INACTIVE;
        case 0b001:
            return (signal){state.val == 0};
        case 0b010:
            return (signal){state.val == 1};
        case 0b011:
            return (signal){state.val == 3};
        case 0b100:
            return ZF;
        case 0b101:
            return ((selReg.val & 1) | ZF.active) ? ACTIVE : INACTIVE;
        case 0b110:
            return selReg.val == 3 ? ACTIVE : INACTIVE;
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
    buff_3.val = (R_IN >> 12) | (buff_3.val & 0x30);
}

void lsh4() {
    R_IN <<= 4;
}

void ld52() {
    buff_3.val = ((R_IN & 0x000F) << 2) | (buff_3.val & 0x3);
}

void ld10() {
    buff_3.val = (R_IN >> 14) | (buff_3.val & 0x3C);
}

void lsh2() {
    R_IN <<= 2;
}

void ld54() {
    buff_3.val = ((R_IN & 0xC) << 2) | (buff_3.val & 0xF);
}

void ldBuff() {
    buff_1.val = R_IN >> 10;
    buff_2.val = (R_IN & 0x1FF) >> 4;
}

void incState() {
    state.val++;
    printf("*** ENC: STATE = %d ***\n", state.val);
}
 
void activate_ld_rom() {
    ld_rom = ACTIVE;
}

void incSelReg() {
    selReg.val++;
}

void reset_ld_rom() {
    ld_rom = INACTIVE;
}

void str_R_OUT() {
    printf("ROUT %d\n", R_OUT);
}

void (*signalActions[])() = {
    &nop,
    &reset,
    &reset_Buff_3,
    &reset_R_OUT,
    &load_R_IN,
    &ld30,
    &lsh4,
    &ld52,
    &ld10,
    &lsh2,
    &ld54,
    &ldBuff,
    &incState,
    &activate_ld_rom,
    &incSelReg,
    &reset_ld_rom,
    &str_R_OUT,
    &activateLoad_uPC,
    &activateRestore_uPC,
    &activateEndProc
};

void encode() {
    
    instructionCodeMemory[28] = (uint10_t){0};
    instructionCodeMemory[29] = (uint10_t){0b1110000000};
    instructionCodeMemory[30] = (uint10_t){0b0010000000};
    instructionCodeMemory[38] = (uint10_t){0b1000000000};
    instructionCodeMemory[43] = (uint10_t){0b1010000000};
    instructionCodeMemory[45] = (uint10_t){0b1000000000};

    opcodeMemory[28] = (uint6_t){INCI};
    opcodeMemory[30] = (uint6_t){CMP};

    loadMicroprogram(controlMemory, instructionCodeMemory, opcodeMemory, &loadAddress, signalActions);
}
