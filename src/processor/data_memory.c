#include <stdio.h>
#include <stdint.h>

#include "internals.h"

static uint16_t dataMemory[DATA_MEMORY_SIZE];

static uint16_t muxedIndex;

static void mux(uint16_t index) {
    if (stackOP.val != 0) {
        muxedIndex = SP_operation();
    } else {
        muxedIndex = index;
    }
}

void store(uint16_t word, uint16_t index) {
    if (str.active || jmpS.active) {
        mux(index);
        dataMemory[muxedIndex] = word;
        printf("DM: Storing %d at %d\n", word, muxedIndex);
    }
}

uint16_t load(uint16_t index) {
    mux(index);
    printf("DM: Loading %d from %d\n", dataMemory[muxedIndex], muxedIndex);
    return dataMemory[muxedIndex];
}