#include "internals.h"

#ifndef uPROGRAM_SEQUENCER_H
#define uPROGRAM_SEQUENCER_H

extern signal ZF;

#define BA_SIZE 8
#define CF_SIZE 15

#define EXTERNAL_CS_COUNT 10

typedef struct {
    unsigned int val : 3;
} uint3_t;

typedef struct {
    unsigned int val : CF_SIZE;
} uint15_t;

typedef struct {
    uint3_t conditionSelect;
    uint15_t BA_CF;
} uInstruction;


#define SIG_NOP 0
#define SIG_STACK_OP_PSH 0x1 
#define SIG_STACK_OP_POP 0x2 
#define SIG_ENC 0x4
#define SIG_IMM_OP 0x8
#define SIG_LDM 0x10
#define SIG_STR 0x20
#define SIG_LSE 0x40
#define SIG_ALU_OP 0x80
#define SIG_ENC_INSTR 0x100
#define SIG_IO_OP 0x200

extern uint3_t conditionSelect;
extern uint8_t branchAddress;
extern uint15_t controlField;

void loadMicroprogram(uInstruction *cm, uint10_t *icm, uint6_t *opm, signal (*la)(), void (*sa[])());
void activateEndProc();
void activateLoad_uPC();
void activateRestore_uPC();
void nop();

#endif