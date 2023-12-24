#include "internals.h"

#ifndef uPROGRAM_SEQUENCER_H
#define uPROGRAM_SEQUENCER_H

extern signal ZF;

#define uPROGRAM_LENGTH 55
#define BA_SIZE 8
#define CF_SIZE 25

#define EXTERNAL_CS_COUNT 8

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

extern uint3_t conditionSelect;
extern uint8_t branchAddress;
extern uint25_t controlField;

void loadMicroprogram(uInstruction *cm, uint10_t *icm, uint6_t *opm, signal (*la)(), void (*sa[])());
void activateEndProc();
void activateLoad_uPC();
void activateRestore_uPC();

#endif