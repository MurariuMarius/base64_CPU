#include <stdio.h>

#include "internals.h"

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

static uint16_t getIndex() {
    uint16_t offset = sign_extend_9_to_16_bits() & 0x003F;
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
    if (selectedRegister & 0x0080) {
        return getSP();
    }
    if ((selectedRegister & 0x0040) != 0) {
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
            // *reg = load(getIndex());
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