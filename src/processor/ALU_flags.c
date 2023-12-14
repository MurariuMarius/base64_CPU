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

static void setOF()
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
            if ((int32_t)(opA + opB) > INT16_MAX || (int32_t)(opA + opB) < INT16_MIN) {
                CF = ACTIVE;
            }

            accumulator = opA + opB;
            break;
        case SUB:
        case SUBI:

            if (opA < opB)
                CF = ACTIVE;
            accumulator = opA - opB;
            break;
        case LSR:
        case LSRI:
            accumulator = opA;
            for (uint8_t i = 0; i < opB; i++) {
                CF.active = (accumulator & 1);
                accumulator >>= 1;
                accumulator &= 0x7FFF;
            }
            break;
        case LSL:
        case LSLI:
        case RSL:
        case RSLI:
            accumulator = opA;
            for (uint8_t i = 0; i < opB; i++) {
                CF.active = NEGATIVE(accumulator);
                accumulator <<= 1;
            }
            break;
        case RSR:
        case RSRI:
            accumulator = opA;

            signal sign = (signal){ NEGATIVE(opA) };

            for (uint8_t i = 0; i < opB; i++) {
                CF.active = (accumulator & 1);
                accumulator <<= 1;
                accumulator |= sign.active << 15;
            }
            break;
        case MUL:
        case MULI:
            if ((int32_t)(opA * opB) > INT16_MAX || (int32_t)(opA * opB) < INT16_MIN) {
                CF = ACTIVE;
            }

            accumulator = opA * opB;
            break;
        case DIV:
        case DIVI:
            if (opB)
                accumulator = opA / opB;
            break;
        case MOD:
        case MODI:
            if (opB)
                accumulator = opA % opB;
            break;
        case AND:
        case ANDI:
            accumulator = opA & opB;
            break;
        case OR:
        case ORI:
            accumulator = opA | opB;
            break;
        case XOR:
        case XORI:
            accumulator = opA ^ opB;
            break;
        case NOT:
        case NOTI:
            accumulator = ~opB;
            break;
        case TST:
        case TSTI:
        case CMP:
        case CMPI:
            NF.active = (opA - opB < 0);
            ZF.active = (opA == opB);
            accumulator = opA;
            break;
        case INCI:
            accumulator = opA + 1;
            break;
        case DECI:
            accumulator = opA - 1;
            break;
        default:
            accumulator = -1;
            ZF = ACTIVE;
            NF = ACTIVE;
            return accumulator;
        }
    }

    setOF();
    
    if (accumulator == 0)
        ZF = ACTIVE;
    else
        switch (instruction.val) {
            case TST:
            case TSTI:
            case CMP:
            case CMPI:
                break;
            default:
                NF.active = accumulator < 0;
        }
    
    printf("ALU: Flags: ZF %d, NF %d, CF %d, OF %d\n", ZF.active, NF.active, CF.active, OF.active);
    printf("ALU: ACC: %d\n", accumulator);

    return accumulator;
}
