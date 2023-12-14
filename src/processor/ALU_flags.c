#include <stdio.h>
#include <stdbool.h>

#include "internals.h"
#include "opcodes.h"

#define POSITIVE(operand) (((operand) & 0x8000) == 0)
#define NEGATIVE(operand) (((operand) & 0x8000) != 0)

int16_t opA;
int16_t opB;
signal ZF,NF,CF,OF;
static int16_t accumulator;

static void check_for_OF()
{
    switch (instruction.val) {
        case ADD:
        case ADDI:
        case SUB:
        case SUBI:
            OF.active = (POSITIVE(opA) && POSITIVE(opB) && NEGATIVE(accumulator)) ||
                        (NEGATIVE(opA) && NEGATIVE(opB) && POSITIVE(accumulator));
            break;
        case MUL:
        case MULI:
            if (POSITIVE(opA)) {
                if (POSITIVE(opB)) {
                    OF.active = opA > (INT16_MAX / opB);
                } else {
                    OF.active = opB < (INT16_MIN / opA);
                }
            } else {
                if (POSITIVE(opB)) {
                    OF.active = opA < (INT16_MIN / opB);
                } else {
                    OF.active = (opA != 0) && (opB < (INT16_MAX / opA));
                }
            }
            break;
        case DIV:
        case DIVI:
        case MOD:
        case MODI:
            OF.active = (opB == 0) || ((opA == INT16_MIN) && (opB == -1));
            break;
        case NOT:
        case NOTI:
            OF.active = (opB == INT16_MIN);
            break;
    }
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

    // printf("FLAGS: jmp %d\n", jmp.active);
}

static uint16_t mux() {
    if (immOp.active) {
        return sign_extend_9_to_16_bits();
    } else {
        return getOperandRegister();
    }
}

uint16_t main_ALU_fcn()
{
    if (!aluOp.active) {
        return accumulator;
    }

    opA = *getSelectedRegister();
    opB = mux();

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
        case ADDI:
            // perform the addition operation
            // check for CF
            if ((uint32_t)(opA + opB) > UINT16_MAX)
                CF = ACTIVE;
            accumulator = opA + opB;
            check_for_OF();
            break;
        case SUB:
        case SUBI:
            // perform the subtraction operation
            // check for CF
            if (opA < opB)
                CF = ACTIVE;
            accumulator = opA - opB;
            check_for_OF();
            break;
        case LSR:
        case LSRI:
            // perform the logical shift right operation
            accumulator = opA >> opB;
            // check for carry
            if (opA & 0b01)
                CF = ACTIVE;
            break;
        case LSL: // these are the same
        case LSLI:
        case RSL:
        case RSLI:
            // perfrom the logical shift left operation
            accumulator = opA << opB;
            break;
        case RSR:
        case RSRI:
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
        case MULI:
            // perform the MUL operation
            accumulator = opA * opB;
            check_for_OF();
            break;
        case DIV:
        case DIVI:
            // perform the DIV operation
            if (opB)
                accumulator = opA / opB;
            check_for_OF();
            break;
        case MOD:
        case MODI:
            // perform the MOD operation
            if (opB)
                accumulator = opA % opB;
            check_for_OF();
            break;
        case AND:
        case ANDI:
            // perform the AND operation
            accumulator = opA & opB;
            break;
        case OR:
        case ORI:
            // perform the OR operation
            accumulator = opA | opB;
            break;
        case XOR:
        case XORI:
            // perform the XOR operation
            accumulator = opA ^ opB;
            break;
        case NOT:
        case NOTI:
            // perform the NOT operation on the first operand
            accumulator = ~opB;
            check_for_OF();
            break;
        case TST: // these are the same
        case TSTI: // these are the same
        case CMP:
        case CMPI:
            // load into result the difference...flags are set after switch
            NF.active = (opA - opB < 0);
            ZF.active = (opA == opB);
            accumulator = opA;
            break;
        case INCI:
            // perform the INC operation on the first operand
            accumulator = opA + 1;
            break;
        case DECI:
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
    else
        switch (instruction.val) {
            case SUB:
            case SUBI:
            case DECI:
                if (accumulator > opA) {
                    NF = ACTIVE;
                }  
        }
    
    printf("ALU: Flags: ZF %d, NF %d, CF %d, OF %d\n", ZF.active, NF.active, CF.active, OF.active);
    printf("ALU: ACC: %d\n", accumulator);

    return accumulator;
}
