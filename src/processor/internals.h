#ifndef INTERNALS_H
#define INTERNALS_H

#define INSTRUCTION_MEMORY_SIZE 500 // bytes


/*  TYPES   */

typedef struct {
    int x : 6;
} uint6_t;

typedef struct {
    int x : 10;
} uint10_t;


/*  INSTRUCTION MEMORY  */

void initializeInstructionMemory(char *binFile);
uint16_t readWordFromInstructionMemory(int index);



#endif