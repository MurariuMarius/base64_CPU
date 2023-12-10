#include <stdio.h>

#include "internals.h"

#define IS_SP(reg) (((reg) & 0x0180) == 0x180)
#define GET_OFFSET(val) ((val) & 0x007F)
#define IS_Y(reg) ((reg) & 0x0080)

signal reg_select;
uint9_t immediate;
uint16_t ext_immediate;

static uint16_t Rx,Ry;

static void Demux2()
{
    uint10_t arguments = getAddressRegisterFromRF();
    reg_select.active = ((arguments.val >> 9) & 0x001); // msb is 1 => true, else false
    immediate.val = arguments.val & 0x01FF;
}

static uint16_t sign_extend_7_to_16_bits(uint16_t value) {
    uint16_t out = 0U;
    int sign = (value & (1 << 6)) != 0;
    for (uint8_t i = 15; i >= 7; i--) {
        out |= (sign << i);
    }
    out |= value;
    return out;
}

static uint16_t getIndex() {
    uint16_t offset = GET_OFFSET(sign_extend_9_to_16_bits());
    offset = sign_extend_7_to_16_bits(offset);
    printf("RF: Offset %04x\n", offset);
    return getOperandRegister() + offset;
}

uint16_t sign_extend_9_to_16_bits()
{
    ext_immediate = 0U;
    uint8_t i;
    for (i = 15; i >= 9; i--)
    {
        ext_immediate |= ((immediate.val & (1 << 9)) << i);
    }
    ext_immediate |= immediate.val;

    return ext_immediate;
}

uint16_t *getSelectedRegister() {
    return reg_select.active ? &Ry : &Rx;
}

uint16_t getOperandRegister() {
    uint16_t selectedRegister = ext_immediate;
    if (IS_SP(selectedRegister)) {
        return getSP();
    }
    if (IS_Y(selectedRegister)) {
        return Ry;
    }
    return Rx;
}

void register_file()
{
    Demux2();
    sign_extend_9_to_16_bits();

    uint16_t *reg = getSelectedRegister();

    if(lse.active)
    {
        if (immOp.active) {
            *reg = ext_immediate;
        } else {
            *reg = getOperandRegister();
        }
    }
    else if(ldm.active)
    {
        if (immOp.active) {
            *reg = load(ext_immediate);
        } else {
            *reg = load(getIndex());
        }
    }
    else if(aluOp.active)
    {
        *reg = main_ALU_fcn();
    }

    else if (immOp.active) {
        store(*reg, ext_immediate);
    } else {
        store(*reg, getIndex());
    }

    printf("RX: %d; RY: %d\n", Rx, Ry);
}