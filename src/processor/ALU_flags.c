#include <stdio.h>
#include <stdbool.h>

#include "internals.h"
#include "opcodes.h"

uint16_t opA;
uint16_t opB;
signal ZF,NF,CF,OF;
static uint16_t accumulator;

void check_for_OF()
{
    signal MSB_A = (signal){(opA & (1 << 7)) != 0};
    signal MSB_B = (signal){(opB & (1 << 7)) != 0};
    signal MSB_R = (signal){(accumulator & (1 << 7)) != 0};
    OF.active = (!(MSB_A.active ^ MSB_B.active) && (MSB_A.active ^ MSB_R.active));
}

void checkFlags() {
    jmp = INACTIVE;
    if (!br_oth.active) {
        return;
    }
    
    uint2_t selector = (uint2_t){instruction.val & 0x003};
    switch(selector.val) {
        // BRZ
        case 0b00:
            jmp = ZF;
            break;
        // BRN
        case 0b01:
            jmp = NF;
            break;
        // BRC
        case 0b10:
            jmp = CF;
            break;
        case 0b11:
            jmp = OF;
    }
}

uint16_t main_ALU_fcn()
{
    if (!aluINSTR.active) {
        return accumulator;
    }

    opA = *getSelectedRegister();
    opB = sign_extend_9_to_16_bits();

    printf("ALU: opA: %d, opB: %d\n", opA, opB);

    ZF = INACTIVE;
    NF = INACTIVE;
    CF = INACTIVE;
    OF = INACTIVE;
    
    if (instruction.val != 0U)
    {
        switch (instruction.val)
        {
        case ADD:
            // perform the addition operation
            // check for CF
            if ((uint32_t)(opA + opB) > UINT16_MAX)
                CF = ACTIVE;
            accumulator = opA + opB;
            check_for_OF();
            break;
        case SUB:
            // perform the subtraction operation
            // check for CF
            if (opA < opB)
                CF = ACTIVE;
            accumulator = opA - opB;
            check_for_OF();
            break;
        case LSR:
            // perform the logical shift right operation
            accumulator = opA >> opB;
            // check for carry
            if (opA & 0b01)
                CF = ACTIVE;
            break;
        case LSL: // these are the same
        case RSL:
            // perfrom the logical shift left operation
            accumulator = opA << opB;
            break;
        case RSR:
            // check for carry
            if (opA & 0b01)
                CF = ACTIVE;
            if (opA & (1 << 15)) // if opA's MSB == 1
            {
                accumulator = opA >> opB;
                while (opB)
                {
                    opB--;
                    accumulator |= (1 << (15 - opB)); // fill the first opB positions in result with opA's MSB
                }
            }
            else
                accumulator = opA >> opB; // first opB positions in result are filled with 0
            break;
        case MOV:
            // perform the MOV operation
            accumulator = opB;
            break;
        case MUL:
            // perform the MUL operation
            accumulator = opA * opB;
            break;
        case DIV:
            // perform the DIV operation
            if (opB)
                accumulator = opA / opB;
            else
                accumulator = -1;
            break;
        case MOD:
            // perform the MOD operation
            if (opB)
                accumulator = opA % opB;
            else
                accumulator = -1;
            break;
        case AND:
            // perform the AND operation
            accumulator = opA & opB;
            break;
        case OR:
            // perform the OR operation
            accumulator = opA | opB;
            break;
        case XOR:
            // perform the XOR operation
            accumulator = opA ^ opB;
            break;
        case NOT:
            // perform the NOT operation on the first operand
            accumulator = ~opA;
            break;
        case TST: // these are the same
        case CMP:
            // load into result the difference...flags are set after switch
            accumulator = opA - opB;
            break;
        case INC:
            // perform the INC operation on the first operand
            accumulator = opA + 1;
            break;
        case DEC:
            // perform the DEC operation on the first operand
            accumulator = opA - 1;
            break;
        default:
            // if the opcode is not recognised, return -1
            accumulator = -1;
        }
    }
    
    if (accumulator == 0)
        ZF = ACTIVE;

    if (accumulator < 0)
        NF = ACTIVE;
    
    printf("ALU: ACC: %d\n", accumulator);

    return accumulator;
}
