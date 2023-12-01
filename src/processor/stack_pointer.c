#include <stdio.h>
#include <stdint.h>

#include "internals.h"

static uint16_t SP = DATA_MEMORY_SIZE;

static void ALU_SP() {
    if (spOp.val == 1U) {
        SP--;
    } else if (spOp.val == 2U) {
        SP++;
    }
}

static uint16_t mux(uint16_t *oldSP, uint16_t *newSP) {
    if (spOp.val == 1U) {
        return *newSP;
    }
    return *oldSP;
}

uint16_t SP_operation() {
    uint16_t oldSP = SP;
    ALU_SP();
    return mux(&oldSP, &SP);
}

