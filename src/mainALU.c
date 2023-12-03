#include "internals.h"
#include "opcodes.h"

extern bool ZF, CF, NF, OF;

uint16_t cmp(uint16_t opA, uint16_t opB)
{
    if (opA < opB)
    {
        return -1;
    }
    if (opA == opB)
    {
        return 0;
    }
    else
        return 1;
}

uint16_t tst(uint16_t operand, uint16_t mask)
{
    return (operand & mask) != 0;
}

void check_for_OF(uint16_t opA, uint16_t opB, uint16_t result, bool *OF)
{
    bool MSB_A = opA & (1 << 7);
    bool MSB_B = opB & (1 << 7);
    bool MSB_R = result & (1 << 7);
    *OF = (!(MSB_A ^ MSB_B) && (MSB_A ^ MSB_R));
}

uint16_t main_ALU_fcn(uint6_t aluOp, uint16_t opA, uint16_t opB)
{
    uint16_t result;

    printf("Operatia executata: %d\n", aluOp.x);

    ZF = false;
    NF = false;
    CF = false;
    OF = false;

    printf("poal");
    if (0U != aluOp.x)
    {
        switch (aluOp.x)
        {
        case ADDI:
            // perform the addition operation
            // check for CF
            if ((uint32_t)(opA + opB) > UINT16_MAX)
            {
                CF = true;
            }
            result = opA + opB;
            check_for_OF(opA, opB, result, &OF);
            break;
        case SUBI:
            // perform the subtraction operation
            // check for CF
            if (opA < opB)
            {
                CF = true;
            }
            result = opA - opB;
            check_for_OF(opA, opB, result, &OF);
            break;
        case LSR:
            // perform the logical shift right operation
            result = opA >> opB;
            // check for carry
            if (opA & 0b01)
            {
                CF = true;
            }
            break;
        case LSL: // these are the same
        case RSL:
            // perfrom the logical shift left operation
            result = opA << opB;
            break;
        case RSR:
            // check for carry
            if (opA & 0b01)
            {
                CF = true;
            }
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
            {
                result = opA >> opB; // first opB positions in result are filled with 0
            }
            break;
        case MOVI:
            // perform the MOV operation
            result = opB;
            break;
        case MULI:
            // perform the MUL operation
            result = opA * opB;
            break;
        case DIVI:
            // perform the DIV operation
            if (opB)
            {
                result = opA / opB;
            }
            else
            {
                result = -1;
            }
            break;
        case MODI:
            // perform the MOD operation
            if (opB)
            {
                result = opA % opB;
            }
            else
            {
                result = -1;
            }
            break;
        case ANDI:
            // perform the AND operation
            result = opA & opB;
            break;
        case ORI:
            // perform the OR operation
            result = opA | opB;
            break;
        case XORI:
            // perform the XOR operation
            result = opA ^ opB;
            break;
        case NOT:
            // perform the NOT operation on the first operand
            result = ~opA;
            break;
        case TST: // these are the same
        case CMPI:
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
    {
        ZF = true;
    }

    if (result < 0)
    {
        NF = true;
    }

    printf("Rezultatul obtinut: %d\n", result);

    return result;
}

void flags_register_fcn(bool *ZF, bool *NF, bool *CF, bool *OF)
{
    static bool ZF_reg;
    static bool NF_reg;
    static bool CF_reg;
    static bool OF_reg;

    ZF_reg = *ZF;
    NF_reg = *NF;
    CF_reg = *CF;
    OF_reg = *OF;

    printf("ZF NF CF OF: %d %d %d %d\n", ZF_reg, NF_reg, CF_reg, OF_reg);
}

bool Mux_flags(bool x0, bool x1, bool x2, bool x3, bool br_oth, bool c1, bool c0)
{
    bool jmp = false;
    uint6_t mask;
    if (br_oth)
    {
        mask.x = 0b000000; // build here the same part of the branches opcodes
        mask.x |= (c1 << 1) | c0;

        switch (mask.x)
        {
        case BRZ:
            x0 ? (jmp = true) : (jmp = false);
            break;
        case BRN:
            x1 ? (jmp = true) : (jmp = false);
            break;
        case BRC:
            x2 ? (jmp = true) : (jmp = false);
            break;
        case BRO:
            x3 ? (jmp = true) : (jmp = false);
            break;
        default:
            break;
        }
    }
    printf("JMP active? %d\n", jmp);
    return jmp;
}

uint16_t accumulator_register_fcn(uint16_t x)
{
    static uint16_t acc_reg;
    acc_reg = x;
    printf("Accumulator has value: %d\n", acc_reg);
    return acc_reg;
}

void Demux_BR(uint6_t opcode, bool *c0, bool *c1, bool br_oth)
{
    if (br_oth)
    {
        // TO SEE WHICH ARE THE POSITION IN BRANCH OPCODEs THAT DIFFER
        (*c0) = opcode.x & (1 << 1);
        (*c1) = opcode.x & (1 << 0);
    }
}