#include <stdio.h>
#include <stdint.h>

#include "internals.h"

static uint16_t SP = DATA_MEMORY_SIZE;

static void ALU_SP() {
    if (stackOP.val == 1U) {
        SP--;
    } else if (stackOP.val == 2U) {
        SP++;
    }
}

static uint16_t mux(uint16_t *oldSP, uint16_t *newSP) {
    if (stackOP.val == 1U) {
        return *newSP;
    }
    return *oldSP;
}

uint16_t SP_operation() {
    uint16_t oldSP = SP;
    ALU_SP();
    printf("SP: %d\n", mux(&oldSP, &SP));
    return mux(&oldSP, &SP);
}

uint16_t getSP() {
    return SP;
}
