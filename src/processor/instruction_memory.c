#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "internals.h"

static uint8_t instructionMemory[INSTRUCTION_MEMORY_SIZE];

static void writeToInstructionMemory(uint16_t word, int index);

void initializeInstructionMemory(char *binFile) {
    FILE *fp = fopen(binFile, "rb");
    if (fp == NULL) {
        printf("Instruction memory: Could not open file %s\n", binFile);
        exit(1);
    }

    int index = 0;
    uint16_t buffer;
    while (fread(&buffer, sizeof(uint8_t), 2, fp) == 2) {
        if (index >= INSTRUCTION_MEMORY_SIZE) {
            printf("%d\n", index);
            printf("Instruction memory: Could not read the whole file\n");
            break;
        }

        writeToInstructionMemory(buffer, index);
        index += 2;
    }

    fclose(fp);
}

uint16_t readWordFromInstructionMemory(int index) {
    return (uint16_t)((instructionMemory[index] << 8) | instructionMemory[index + 1]);
}

static void writeToInstructionMemory(uint16_t word, int index) {

    /**
     * Assuming that the simulator's architecure has little endian ordering.
     * The binary input file and the instruction memory use big endian ordering.
    */

    instructionMemory[index] = (uint8_t)(word & 0x00FF);
    instructionMemory[index + 1] = (uint8_t)(word >> 8);
}
