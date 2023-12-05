#ifndef OPCODES_H
#define OPCODES_H

#define LDR 0b000001
#define STR 0b000010

#define BRZ 0b000100
#define BRN 0b000101
#define BRC 0b000110
#define BRO 0b000111
#define BRA 0b001000
#define JMP 0b010000
#define RET 0b010001

#define PSH 0b010010
#define POP 0b010011

#define ADD 0b011100
#define SUB 0b011101
#define INC 0b011110
#define DEC 0b011111
#define CMP 0b100000
#define TST 0b100001
#define LSR 0b100010
#define LSL 0b100011
#define RSR 0b100100
#define RSL 0b100101
#define MOV 0b100110
#define MUL 0b100111
#define DIV 0b101000
#define MOD 0b101001
#define AND 0b101010
#define OR  0b101011
#define XOR 0b101100
#define NOT 0b101101

#define ADDI 0b101110
#define SUBI 0b101111
#define CMPI 0b110000
#define TSTI 0b110001
#define LSRI 0b110010
#define LSLI 0b110011
#define RSRI 0b110100
#define RSLI 0b110101
#define MOVI 0b110110
#define MULI 0b110111
#define DIVI 0b111000
#define MODI 0b111001
#define ANDI 0b111010
#define ORI  0b111011
#define XORI 0b111100
#define NOTI 0b111101
#define INCI 0b111110
#define DECI 0b111111

#define HLT 0b000000

#endif