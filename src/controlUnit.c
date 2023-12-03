#include "header.h"
#include "opcodes.h"

void control_unit_fcn(uint6_t instruction, bool *next, bool *br_oth, uint6_t *aluOp, bool *LSE, bool *LDM, bool *LACC, bool *ABS, uint2_t *spO)
{
    // SP OP
    if (PSH == instruction.x)
    { // PSH
        spO->x = 1U;
    }
    else if (POP == instruction.x)
    { // POP
        spO->x = 2U;
    }

    if (BRZ <= instruction.x && RET >= instruction.x)
    { // BRANCH
        (*br_oth) = true;
        // if BRA, also activate ABS (always branch signal)
        if (BRA == instruction.x)
        {
            (*ABS) = true;
        }
        else
        {
            (*ABS) = false;
        }
        (*next) = false;
    }
    else
    {
        (*br_oth) = false;
        (*next) = true;
    }

    if (ADDC <= instruction.x && TSTC >= instruction.x)
    { // ARITM OR LOGIC OP
        (*aluOp).x = instruction.x;

        // if there is an opcode with immediate value => we need to perform an ALU op with the immd value =>
        //=> extend it from 9bit to 16bit
        if (0b010011 <= instruction.x && 0b011101 >= instruction.x)
        {
            (*LSE) = true;
        }
        else
        {
            (*LSE) = false;
        }
    }
    else
    {
        (*aluOp).x = 0U;
    }

    if (0b011111 == instruction.x)
    { // LDR
        (*LDM) = 1U;
    }
}