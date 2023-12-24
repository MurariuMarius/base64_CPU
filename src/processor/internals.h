#include <stdint.h>

#ifndef INTERNALS_H
#define INTERNALS_H

#define INSTRUCTION_MEMORY_SIZE 500 // bytes
#define DATA_MEMORY_SIZE 512 // 2 ^ 9 words

#define ACTIVE (signal){1}
#define INACTIVE (signal){0}

#define IO_IN INACTIVE
#define IO_OUT ACTIVE

/*  TYPES   */

typedef struct {
    unsigned int active : 1;
} signal;

typedef struct {
    unsigned int val : 2;
} uint2_t;

typedef struct {
    unsigned int val : 6;
} uint6_t;

typedef struct {
    unsigned int val : 9;
} uint9_t;

typedef struct {
    unsigned int val : 10;
} uint10_t;


/*  SIGNALS  */
extern signal next;
extern signal br_always;
extern signal jmp;
extern signal br_oth;
extern signal end;
extern signal aluOp;
extern signal lse;
extern signal ldm;
extern signal str;
extern signal immOp;
extern signal jmpS;
extern signal ret;
extern uint2_t stackOP;
extern signal send;
extern signal IO_type;
extern signal IO_Op;
extern signal enc;

/* BUSSES */
extern uint6_t instruction;
extern uint16_t IO_data;

/*  INSTRUCTION MEMORY  */

void initializeInstructionMemory(char *binFile);
uint16_t readWordFromInstructionMemory(uint16_t index);
uint10_t getAddressRegisterFromRF();
uint16_t getAddressRegisterFromPC();

/*  PROGRAM COUNTER  */
void nextPC();

/*  CONTROL UNIT  */
void CU_handleNextInstruction();

/*  DATA MEMORY  */
void store(uint16_t word, uint16_t index);
uint16_t load(uint16_t index);

/*  STACK POINTER  */
uint16_t SP_operation();
uint16_t getSP();

/*  ALU  */
uint16_t main_ALU_fcn();
void checkFlags();

/*  REGISTER FILE  */
void register_file();
uint16_t *getSelectedRegister();
uint16_t sign_extend_9_to_16_bits();
uint16_t getOperandRegister();
uint16_t getIndex();

/*  MICROPRORGAM SEQUENCER  */
uint10_t getInstruction();

/*  ENCODER  */
void encode();

/* IO  */
void io();

/* DRIVER */
void read();
void request(signal type);
void write();

// void initializePC();
// void testExtendUnit(uint10_t in);

#endif