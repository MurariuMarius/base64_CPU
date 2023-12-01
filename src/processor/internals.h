#include <stdint.h>

#ifndef INTERNALS_H
#define INTERNALS_H

#define INSTRUCTION_MEMORY_SIZE 500 // bytes

#define ACTIVE (signal){1}
#define INACTIVE (signal){0}

/*  TYPES   */

typedef struct {
    unsigned int active : 1;
} signal;

typedef struct {
    unsigned int val : 6;
} uint6_t;

typedef struct {
    unsigned int val : 10;
} uint10_t;


/*  SIGNALS  */
extern signal next;
extern signal bra;
extern signal jmp;
extern signal branch_other;
extern signal end;

/*  INSTRUCTION MEMORY  */

void initializeInstructionMemory(char *binFile);
uint16_t readWordFromInstructionMemory(int index);
uint10_t getAddressRegisterFromRF();
uint16_t getAddressRegisterFromPC();

/*  PROGRAM COUNTER  */
void nextPC();

/*  CONTROL UNIT  */
void CU_handleNextInstruction(uint6_t opCode);

// void initializePC();
// void testExtendUnit(uint10_t in);

#endif