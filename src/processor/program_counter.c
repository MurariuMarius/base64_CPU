#include <stdio.h>
#include <stdint.h>

#include "internals.h"

static uint16_t PC;

uint16_t incrementPC() {
    if (next.active) {
        PC += 2;
    }
    return PC;
}

void nextPC() {
    readWordFromInstructionMemory(PC);

    uint16_t branchAddress = getAddressRegisterFromPC();
    signal branch = (signal){(jmp.active & br_oth.active) | br_always.active};

    // printf("PC: next: %d branch %d to %04x\n", next.active, branch.active, branchAddress);

    if (branch.active) {
        PC = branchAddress;
    } else {
        PC = incrementPC();
    }
}
