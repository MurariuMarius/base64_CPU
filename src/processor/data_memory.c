#include <stdint.h>

#include "internals.h"

static uint16_t dataMemory[DATA_MEMORY_SIZE];

void store(uint16_t word, uint16_t index) {
    dataMemory[index] = word;
}

uint16_t load(uint16_t index) {
    return dataMemory[index];
}