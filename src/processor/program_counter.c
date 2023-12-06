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

    // printf("PC: next: %d JMPS %d branch %d to %04x\n", next.active, jmpS.active, branch.active, branchAddress);

    if (branch.active) {
        if (jmpS.active) {
            store(PC, -1);
        }
        PC = branchAddress;
    } else {
        if (ret.active) {
            PC = load(-1);
        }
        PC = incrementPC();
    }
}
