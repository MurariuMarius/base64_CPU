#include <stdint.h>

#include "internals.h"

static uint16_t dataMemory[DATA_MEMORY_SIZE];

void store(uint16_t word, uint9_t index) {
    dataMemory[index.val] = word;
}

uint16_t load(uint9_t index) {
    return dataMemory[index.val];
}