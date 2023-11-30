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

void setPC() {
    readWordFromInstructionMemory(PC);

    uint16_t branchAddress = getAddressRegisterFromPC();
    signal branch = (signal){jmp.active | bra.active};

    if (branch.active) {
        PC = branchAddress;
    } else {
        PC = incrementPC();
    }
}
