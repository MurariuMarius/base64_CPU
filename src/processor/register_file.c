#include <stdio.h>

#include "internals.h"

signal reg_select;
uint9_t immediate;
uint16_t ext_immediate;

static uint16_t Rx,Ry;

void Demux2()
{
    uint10_t arguments = getAddressRegisterFromRF();
    reg_select.active = ((arguments.val >> 9) & 0x001); // msb is 1 => true, else false
    immediate.val = arguments.val & 0x01FF;
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
    if ((ext_immediate & 0x1) != 0) {
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
            printf("Moving %d\n", getOperandRegister());
        }
    }
    if(ldm.active)
    {
        *reg = load(ext_immediate);
    }
    if(aluINSTR.active)
    {
        *reg = main_ALU_fcn();
    }

    store(*reg, ext_immediate);

    printf("RX: %d; RY: %d\n", Rx, Ry);
}