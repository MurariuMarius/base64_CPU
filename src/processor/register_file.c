#include <stdio.h>

#include "internals.h"

signal reg_select;
uint16_t accumulator;
uint16_t data_memory;
signal lse;
signal ldm;
signal lacc;
uint9_t immediate;
uint16_t ext_immediate;
uint10_t arguments;

void Demux2()
{
    reg_select.active = ((arguments.val >> 9) & 0x001); // msb is 1 => true, else false
    immediate.val = arguments.val & 0x01FF;
}

void sign_extend_9_to_16_bits()
{
    ext_immediate = 0U;
    uint8_t i;
    for (i = 15; i >= 9; i--)
    {
        ext_immediate |= ((immediate.val & (1 << 9)) << i);
    }
    ext_immediate |= immediate.val;
}

uint16_t register_file()
{
    uint16_t Rx,Ry;
    
    if(lse.active)
    {
        if(reg_select.active)
            Rx = ext_immediate;
        else
            Ry = ext_immediate;
    }
    if(ldm.active)
    {
        if(reg_select.active)
            Rx = data_memory;
        else
            Ry = data_memory;
    }
    if(lacc.active)
    {
        if(reg_select.active)
            Rx = accumulator;
        else
            Ry = accumulator;
    }

    if(reg_select.active)
            return Rx;
        else
            return Ry;
}
