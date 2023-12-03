#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

// uint pe 6 biti...idk daca merge dar am nevoie pt opcode
typedef struct uint6_t
{
    int x : 6;
} uint6_t;
typedef struct uint10_t
{
    int x : 10;
} uint10_t;
typedef struct uint9_t
{
    int x : 9;
} uint9_t;

typedef struct uint2_t
{
    int x : 3;
} uint2_t;

uint16_t ALU_SP_fcn(uint16_t stack_pointer, uint2_t *spO);
uint16_t stack_pointer_fcn(uint2_t *spO);

void control_unit_fcn(uint6_t instruction, bool *next, bool *br_oth, uint6_t *aluOp, bool *LSE, bool *LDM, bool *LACC, bool *ABS, uint2_t *spO);
void flags_register_fcn(bool *ZF, bool *NF, bool *CF, bool *OF);

uint16_t instruction_memory_fcn(uint16_t program_counter);
uint6_t instruction_register_fcn(uint16_t instruction);
uint10_t arguments_register_fcn(uint16_t instruction);
void read_machine_code();

// uint16_t program_counter_fcn(double clk, uint16_t PC_from_ALU, bool always_branch_signal, bool jmp_signal, uint16_t address);
uint16_t program_counter_fcn(double clk, bool always_branch_signal, bool jmp_signal, uint16_t address);
uint16_t ALU_PC_fcn(uint16_t PC_value, bool next_signal);
void Demux(bool BR_or_OTH, uint10_t arguments, uint10_t *Y0, uint10_t *Y1);
uint16_t sign_extend_10_to_16_fcn(double clk, uint10_t input);

uint16_t main_ALU_fcn(uint6_t aluOp, uint16_t opA, uint16_t opB);
uint16_t accumulator_register_fcn(uint16_t x);
void flags_register_fcn(bool *ZF, bool *NF, bool *CF, bool *OF);
bool Mux_flags(bool x0, bool x1, bool x2, bool x3, bool br_oth, bool c1, bool c0);
void Demux_BR(uint6_t opcode, bool *c0, bool *c1, bool br_oth);
uint16_t data_memory_fcn(uint16_t register_value, uint16_t stack_pointer_value, uint16_t immediate_sgn_ext);

uint16_t register_file_fcn(double clk, bool reg_select, uint16_t accumulator, uint16_t data_memory, bool LSE, bool LDM, bool LACC, uint16_t ext_immediate);
uint16_t sign_extend_9_to_16_fcn(double clk, uint9_t input);
void Demux2(uint10_t arguments, bool *reg_sel, uint9_t *immediate);

uint16_t sgn_extend_fct(int input);
uint16_t sgn_extend1_fct(uint8_t input);
// FOR DEBUG PURPOSES
void show_bits(uint16_t a);