#include <stdio.h>
#include <stdbool.h>

#include "internals.h"
#include "opcodes.h"

uint6_t aluOp;
uint16_t opA;
uint16_t opB;
signal ZF,NF,CF,OF;
uint16_t result;

void check_for_OF()
{
    signal MSB_A = (signal){(opA & (1 << 7)) != 0};
    signal MSB_B = (signal){(opB & (1 << 7)) != 0};
    signal MSB_R = (signal){(result & (1 << 7)) != 0};
    OF.active = (!(MSB_A.active ^ MSB_B.active) && (MSB_A.active ^ MSB_R.active));
}

void main_ALU_fcn()
{
    ZF = INACTIVE;
    NF = INACTIVE;
    CF = INACTIVE;
    OF = INACTIVE;
    
    if (aluOp.val != 0U)
    {
        switch (aluOp.val)
        {
        case ADD:
            // perform the addition operation
            // check for CF
            if ((uint32_t)(opA + opB) > UINT16_MAX)
                CF = ACTIVE;
            result = opA + opB;
            check_for_OF();
            break;
        case SUB:
            // perform the subtraction operation
            // check for CF
            if (opA < opB)
                CF = ACTIVE;
            result = opA - opB;
            check_for_OF();
            break;
        case LSR:
            // perform the logical shift right operation
            result = opA >> opB;
            // check for carry
            if (opA & 0b01)
                CF = ACTIVE;
            break;
        case LSL: // these are the same
        case RSL:
            // perfrom the logical shift left operation
            result = opA << opB;
            break;
        case RSR:
            // check for carry
            if (opA & 0b01)
                CF = ACTIVE;
            if (opA & (1 << 15)) // if opA's MSB == 1
            {
                result = opA >> opB;
                while (opB)
                {
                    opB--;
                    result |= (1 << (15 - opB)); // fill the first opB positions in result with opA's MSB
                }
            }
            else
                result = opA >> opB; // first opB positions in result are filled with 0
            break;
        case MOV:
            // perform the MOV operation
            result = opB;
            break;
        case MUL:
            // perform the MUL operation
            result = opA * opB;
            break;
        case DIV:
            // perform the DIV operation
            if (opB)
                result = opA / opB;
            else
                result = -1;
            break;
        case MOD:
            // perform the MOD operation
            if (opB)
                result = opA % opB;
            else
                result = -1;
            break;
        case AND:
            // perform the AND operation
            result = opA & opB;
            break;
        case OR:
            // perform the OR operation
            result = opA | opB;
            break;
        case XOR:
            // perform the XOR operation
            result = opA ^ opB;
            break;
        case NOT:
            // perform the NOT operation on the first operand
            result = ~opA;
            break;
        case TST: // these are the same
        case CMP:
            // load into result the difference...flags are set after switch
            result = opA - opB;
            break;
        case INC:
            // perform the INC operation on the first operand
            result = opA + 1;
            break;
        case DEC:
            // perform the DEC operation on the first operand
            result = opA - 1;
            break;
        default:
            // if the opcode is not recognised, return -1
            result = -1;
        }
    }
    
    if (result == 0)
        ZF = ACTIVE;

    if (result < 0)
        NF = ACTIVE;
    
}
