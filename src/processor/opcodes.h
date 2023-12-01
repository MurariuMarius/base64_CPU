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

#define ADD 0b100000
#define SUB 0b100001
#define LSR 0b100010
#define LSL 0b100011
#define RSR 0b100100
#define RSL 0b100101
#define MOV 0b100110
#define MUL 0b100111
#define DIV 0b101000
#define MOD 0b101001
#define AND 0b101010
#define OR 0b101011
#define XOR 0b101100
#define NOT 0b101101
#define CMP 0b101110
#define TST 0b101111
#define INC 0b110000
#define DEC 0b110001

#define HLT 0b000000

#endif