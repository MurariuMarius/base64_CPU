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
#define SIG_INC_SEL_BYTE    0b10001
#define SIG_RESET_SEL_REG   0b10010
#define SIG_LOAD_uPC        0b10011
#define SIG_RESTORE_uPC     0b10100
#define SIG_END_PROC        0b10101

/*  LABELS  */
#define PREPARE 0
#define BEGIN 12
#define S0 20
#define S1 24
#define PUSH 30
#define NEXT 35
#define INC_STATE 40
#define END 44
#define CLEANUP 47
#define PROC_OUTBUS 52
#define STORE 56
#define ADV_SEL_REG 59
#define EXIT 63

#define uPROGRAM_LENGTH 70

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
    (uint3_t){0}, generateCF(SIG_ENC | SIG_IMM_OP | SIG_LSE, SIG_NOP),                         // MOV Y, #90
    (uint3_t){0}, generateCF(SIG_ENC | SIG_IMM_OP | SIG_ALU_OP | SIG_ENC_INSTR, SIG_NOP),      // LSL Y, #2
    (uint3_t){0}, generateCF(SIG_ENC | SIG_IO_OP | SIG_STR | SIG_ENC_INSTR, SIG_NOP),          // IN Y, 1
    (uint3_t){0}, generateCF(SIG_ENC | SIG_IMM_OP | SIG_LDM, SIG_NOP),                         // LDR X, 0
    (uint3_t){0}, generateCF(SIG_ENC | SIG_IMM_OP | SIG_ALU_OP | SIG_ENC_INSTR, SIG_NOP),      // ADD X, #0
    (uint3_t){4}, generateBA(EXIT),                                                            // if ZF go to EXIT
    (uint3_t){0}, generateCF(SIG_ENC | SIG_ALU_OP | SIG_ENC_INSTR, SIG_NOP),                   // SUB Y, X
    (uint3_t){0}, generateCF(SIG_ENC | SIG_IMM_OP | SIG_LSE, SIG_NOP),                         // MOV X, #90
    (uint3_t){0}, generateCF(SIG_ENC | SIG_IMM_OP | SIG_ALU_OP | SIG_ENC_INSTR, SIG_NOP),      // LSL X, #2
    (uint3_t){0}, generateCF(SIG_STACK_OP_PSH | SIG_ENC | SIG_IMM_OP | SIG_STR, SIG_RESET),    // PSH Y, reset    
    (uint3_t){0}, generateCF(SIG_ENC | SIG_IMM_OP | SIG_LSE, SIG_RESET_BUFF_3),                // MOV Y, #120, reset_Buff_3
    (uint3_t){0}, generateCF(SIG_ENC | SIG_IMM_OP | SIG_ALU_OP | SIG_ENC_INSTR,
                             SIG_RESET_R_OUT),                                                 // LSL Y, #2, reset_R_OUT
// BEGIN: 12                    
    (uint3_t){0}, generateCF(SIG_ENC, SIG_LD_R_IN),                                            // LDR R_IN, [X]
// SCAN: 13
    (uint3_t){1}, generateBA(S0),                                                              // if state = 0 go to S0
    (uint3_t){2}, generateBA(S1),                                                              // if state = 1 go to S1
    (uint3_t){0}, generateCF(SIG_NOP, SIG_LD30),                                               // BUFF3[3:0] = R_IN[15:12]
    (uint3_t){0}, generateCF(SIG_NOP, SIG_LSH4),                                               // LSH R_IN, #4
    (uint3_t){0}, generateCF(SIG_NOP, SIG_LOAD_uPC),                                           // load_uPC
    (uint3_t){7}, generateBA(PROC_OUTBUS),                                                     // call outbus
    (uint3_t){7}, generateBA(PUSH),                                                            // go to PUSH
// S0: 20                      
    (uint3_t){0}, generateCF(SIG_NOP, SIG_RESET_SEL_REG),                                      // reset_sel_reg
    (uint3_t){0}, generateCF(SIG_NOP, SIG_RESET_BUFF_3),                                       // reset_Buff_3
    (uint3_t){0}, generateCF(SIG_NOP, SIG_LD52),                                               // BUFF3[5:2] = R_IN[3:0]
    (uint3_t){7}, generateBA(PUSH),                                                            // go to PUSH
// S1: 24                      
    (uint3_t){0}, generateCF(SIG_NOP, SIG_LD10),                                               // BUFF3[1:0] = R_IN[15:14]
    (uint3_t){0}, generateCF(SIG_NOP, SIG_LSH2),                                               // LSH R_IN, #2
    (uint3_t){0}, generateCF(SIG_NOP, SIG_LOAD_uPC),                                           // load_uPC
    (uint3_t){7}, generateBA(PROC_OUTBUS),                                                     // call outbus
    (uint3_t){0}, generateCF(SIG_NOP, SIG_RESET_BUFF_3),                                       // reset_Buff_3
    (uint3_t){0}, generateCF(SIG_NOP, SIG_LD54),                                               // BUFF3[5:4] = R_IN[3:2]
// PSH: 30
    (uint3_t){0}, generateCF(SIG_NOP, SIG_LD_BUFF),                                            // BUFF1 = R_IN[15:10], BUFF2 = R_IN[9:4]
// OUTPUT: 31
    (uint3_t){0}, generateCF(SIG_NOP, SIG_LOAD_uPC),                                           // load_uPC
    (uint3_t){7}, generateBA(PROC_OUTBUS),                                                     // call outbus
    (uint3_t){0}, generateCF(SIG_NOP, SIG_LOAD_uPC),                                           // load_uPC
    (uint3_t){7}, generateBA(PROC_OUTBUS),                                                     // call outbus
// NEXT: 35
    (uint3_t){0}, generateCF(SIG_ENC | SIG_IMM_OP | SIG_ALU_OP | SIG_ENC_INSTR, SIG_NOP),      // DEC X
    (uint3_t){0}, generateCF(SIG_ENC | SIG_STR, SIG_NOP),                                      // STR Y, [SP-1]
    (uint3_t){0}, generateCF(SIG_ENC | SIG_LDM, SIG_NOP),                                      // LDR Y, [SP]
    (uint3_t){0}, generateCF(SIG_ENC | SIG_ALU_OP | SIG_ENC_INSTR, SIG_NOP),                   // CMP X, Y
    (uint3_t){0}, generateCF(SIG_ENC | SIG_LDM, SIG_NOP),                                      // LDR Y, [SP-1]
// INC_STATE: 40
    (uint3_t){0}, generateCF(SIG_NOP, SIG_INC_STATE),                                          // state++  
    (uint3_t){3}, generateBA(INC_STATE),                                                       // if state = 3 go to INC_STATE 
    (uint3_t){4}, generateBA(END),                                                             // if ZF = 1 go to END
    (uint3_t){7}, generateBA(BEGIN),                                                           // go to BEGIN
// END: 44
    (uint3_t){1}, generateBA(CLEANUP),                                                         // if STATE = 0 go to CLEANUP
    (uint3_t){0}, generateCF(SIG_NOP, SIG_LOAD_uPC),                                           // load_uPC
    (uint3_t){7}, generateBA(PROC_OUTBUS),                                                     // call outbus
// CLEANUP: 47
    (uint3_t){0}, generateCF(SIG_ENC | SIG_IMM_OP | SIG_LSE, SIG_NOP),                         // MOV X, #120
    (uint3_t){0}, generateCF(SIG_ENC | SIG_IMM_OP | SIG_ALU_OP | SIG_ENC_INSTR, SIG_NOP),      // LSL X, #2
    (uint3_t){0}, generateCF(SIG_ENC | SIG_IO_OP | SIG_ENC_INSTR, SIG_NOP),                    // OUT X, 1
    (uint3_t){0}, generateCF(SIG_STACK_OP_POP | SIG_ENC | SIG_IMM_OP | SIG_LDM, SIG_NOP),      // POP Y
    (uint3_t){7}, generateBA(PREPARE),                                                         // go to PREPARE
// PROC OUTBUS: 52
    (uint3_t){0}, generateCF(SIG_NOP, SIG_LD_ROM),                                             // ld_rom = 1
    (uint3_t){0}, generateCF(SIG_NOP, SIG_INC_SEL_BYTE),                                       // selByte++
    (uint3_t){5}, generateBA(STORE),                                                           // if ~selByte or ZF go to STORE
    (uint3_t){7}, generateBA(ADV_SEL_REG),                                                     // go to ADV_SEL_REG
// STORE: 56
    (uint3_t){0}, generateCF(SIG_ENC | SIG_STR, SIG_STR_R_OUT),                                // STR R_OUT, [Y]
    (uint3_t){0}, generateCF(SIG_NOP, SIG_RESET_R_OUT)    ,                                    // R_OUT = 0 
    (uint3_t){0}, generateCF(SIG_ENC | SIG_IMM_OP | SIG_ALU_OP | SIG_ENC_INSTR, SIG_NOP),      // DEC Y
// ADV_SEL_REG: 59
    (uint3_t){0}, generateCF(SIG_NOP, SIG_INC_SEL_REG),                                        // selReg++
    (uint3_t){6}, generateBA(ADV_SEL_REG),                                                     // if selReg = 3 go to ADV_SEL_REG
    (uint3_t){0}, generateCF(SIG_NOP, SIG_RESET_LD_ROM),                                       // ld_rom = 0
    (uint3_t){0}, generateCF(SIG_NOP, SIG_RESTORE_uPC),                                        // restore_uPC
// EXIT: 63
    (uint3_t){0}, generateCF(SIG_NOP, SIG_END_PROC)                                            // endProc
};

static uint10_t instructionCodeMemory[uPROGRAM_LENGTH] = {
    {0b1000000000}, // LDR Y, 0
    {0b1000000000}, // PSH Y
    {0b0101101000}, // MOV X, #360
    {0b1111100000}, // MOV Y, #480
    {0}, // reset
    {0b0000000000}, // LDR R_IN, [X]
    {0}, // if state = 0
    {0}, // if state = 1
    {0}, // BUFF3[3:0] = R_IN[15:12]
    {0}, // endProc
};

static uint6_t opcodeMemory[uPROGRAM_LENGTH];

static void initializeMemory() {
    instructionCodeMemory[PREPARE] = (uint10_t){0b1001011010};
    instructionCodeMemory[PREPARE + 1] = (uint10_t){0b1000000010};
    instructionCodeMemory[PREPARE + 2] = (uint10_t){0b0010000001};
    instructionCodeMemory[PREPARE + 3] = (uint10_t){0b0000000000};
    instructionCodeMemory[PREPARE + 4] = (uint10_t){0b0000000000};
    instructionCodeMemory[PREPARE + 6] = (uint10_t){0b1000000000};
    instructionCodeMemory[PREPARE + 7] = (uint10_t){0b0001011010};
    instructionCodeMemory[PREPARE + 8] = (uint10_t){0b0000000010};
    instructionCodeMemory[PREPARE + 9] = (uint10_t){0b1000000000};
    instructionCodeMemory[PREPARE + 10] = (uint10_t){0b1001111000};
    instructionCodeMemory[PREPARE + 11] = (uint10_t){0b1000000010};

    instructionCodeMemory[NEXT] = (uint10_t){0};

    instructionCodeMemory[NEXT + 1] = (uint10_t){0b1111111111};
    instructionCodeMemory[NEXT + 2] = (uint10_t){0b1110000000};
    instructionCodeMemory[NEXT + 3] = (uint10_t){0b0010000000};
    instructionCodeMemory[NEXT + 4] = (uint10_t){0b1111111111};

    instructionCodeMemory[CLEANUP] = (uint10_t){0b1000000000};
    instructionCodeMemory[CLEANUP] = (uint10_t){0b0001111000};
    instructionCodeMemory[CLEANUP + 1] = (uint10_t){0b0000000010};
    instructionCodeMemory[CLEANUP + 2] = (uint10_t){0b1000000001}; // Y contains the minimum index to be outputted

    instructionCodeMemory[STORE] = (uint10_t){0b1010000000};
    instructionCodeMemory[STORE + 2] = (uint10_t){0b1000000000};

    opcodeMemory[PREPARE + 1] = (uint6_t){LSLI};
    opcodeMemory[PREPARE + 2] = (uint6_t){IN};
    opcodeMemory[PREPARE + 4] = (uint6_t){ADDI};
    opcodeMemory[PREPARE + 6] = (uint6_t){SUB};
    opcodeMemory[PREPARE + 8] = (uint6_t){LSLI};
    opcodeMemory[PREPARE + 11] = (uint6_t){LSLI};
    opcodeMemory[NEXT] = (uint6_t){DECI};
    opcodeMemory[NEXT + 3] = (uint6_t){CMP};
    opcodeMemory[CLEANUP + 1] = (uint6_t){LSLI};
    opcodeMemory[CLEANUP + 2] = (uint6_t){OUT};
    opcodeMemory[STORE + 2] = (uint6_t){DECI};
}

static uint16_t R_IN;
static uint16_t R_OUT;

static uint6_t buff_1;
static uint6_t buff_2;
static uint6_t buff_3;

static uint2_t selReg;
static signal selByte;
static uint2_t state;
static signal ld_rom;

static uint8_t encodingTable[] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
    'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
    'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3',
    '4', '5', '6', '7', '8', '9', '+', '/'
};

static uint6_t muxBuffers() {
    switch(selReg.val) {
        case 0b00:
            printf("ENC: ROUT BUFF1\n");
            return buff_1;
        case 0b01:
            printf("ENC: ROUT BUFF2\n");
            return buff_2;
        case 0b10:
            printf("ENC: ROUT BUFF3\n");
            return buff_3;
    }
    printf("ENC: *** MUX ERROR *** ");
    return (uint6_t){-1};
}

extern signal incompleteRead;

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
            printf("ENC: ROUT COND %d %d %d %d\n", !selByte.active, ZF.active, incompleteRead.active, (!selByte.active && !ZF.active) || (ZF.active && !incompleteRead.active));
            return ((!selByte.active && !ZF.active) || (ZF.active && !incompleteRead.active)) ? ACTIVE : INACTIVE;
        case 0b110:
            return selReg.val == 3 ? ACTIVE : INACTIVE;
        case 0b111:
            return ACTIVE;
    }
    return INACTIVE;
}

void reset() {
    printf("ENC: Reset selReg, buff1/2, state, selByte\n");
    selReg.val = 0;
    buff_1.val = 0;
    buff_2.val = 0;
    state.val = 0;
    selByte = INACTIVE;
}

void reset_Buff_3() {
    printf("ENC: Reset buff3\n");
    buff_3.val = 0;
}

void reset_R_OUT() {
    printf("ENC: Reset R_OUT\n");
    R_OUT = 0;
}

void load_R_IN() {
    R_IN = load(getIndex());
    printf("ENC: LOAD: R_IN %x\n", R_IN);
}

void ld30() {
    buff_3.val = (R_IN >> 12) | (buff_3.val & 0x30);
    printf("ENC: BUFF_3 ld30: %x\n", buff_3.val);
}

void lsh4() {
    R_IN <<= 4;
    printf("ENC: R_IN lsh4 %x\n", R_IN);
}

void ld52() {
    buff_3.val = ((R_IN & 0x000F) << 2) | (buff_3.val & 0x3);
    printf("ENC: BUFF_3 ld52: %x\n", buff_3.val);
}

void ld10() {
    buff_3.val = (R_IN >> 14) | (buff_3.val & 0x3C);
    printf("ENC: BUFF_3 ld10: %x\n", buff_3.val);
}

void lsh2() {
    R_IN <<= 2;
    printf("ENC: R_IN lsh2 %x\n", R_IN);
}

void ld54() {
    buff_3.val = ((R_IN & 0xC) << 2) | (buff_3.val & 0xF);
    printf("ENC: BUFF_3 ld54: %x\n", buff_3.val);
}

void ldBuff() {
    buff_1.val = R_IN >> 10;
    buff_2.val = (R_IN & 0x3F0) >> 4;
    printf("ENC: BUFF_1: %x\n", buff_1.val);
    printf("ENC: BUFF_2: %x\n", buff_2.val);
}

void incState() {
    state.val++;
    printf("*** ENC: STATE = %d ***\n", state.val);
}

signal receivedZero;

void activate_ld_rom() {
    ld_rom = ACTIVE;
    receivedZero = INACTIVE;
    uint8_t encodedChar = encodingTable[muxBuffers().val];
    if (selByte.active) {
        if ((muxBuffers().val == buff_2.val) && (buff_2.val == 0)) {
            receivedZero = ACTIVE;
        }
        R_OUT = ((uint16_t)encodedChar) | (R_OUT & 0xFF00);
    } else {
        R_OUT = (((uint16_t)encodedChar) << 8) | (R_OUT & 0x00FF);
    }
    printf("ENC: ROUT %d %x %04x\n", selByte.active, encodedChar, R_OUT);
}

void incSelReg() {
    selReg.val++;
}

void reset_ld_rom() {
    ld_rom = INACTIVE;
}

void str_R_OUT() {
    printf("ENC: ROUT STORE %x\n", R_OUT);
    store(R_OUT, getIndex());
}

void incSelByte() {
    selByte.active++;
}

void resetSelReg() {
    selReg.val = 0;
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
    &incSelByte,
    &resetSelReg,
    &activateLoad_uPC,
    &activateRestore_uPC,
    &activateEndProc
};

void encode() {
    initializeMemory();
    loadMicroprogram(controlMemory, instructionCodeMemory, opcodeMemory, &loadAddress, signalActions);
}
