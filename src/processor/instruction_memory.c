#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "internals.h"

static uint8_t instructionMemory[INSTRUCTION_MEMORY_SIZE];

static uint6_t instructionRegister;
static uint10_t addressRegister;

static uint10_t Y0;
static uint10_t Y1;

static uint16_t extendSignFrom10To16(uint10_t input);
static void writeToInstructionMemory(uint16_t word, uint16_t index);
static void demux();

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

uint16_t getAddressRegisterFromPC() {
    demux(&Y0, &Y1);
    return extendSignFrom10To16(Y0);
}

uint10_t getAddressRegisterFromRF() {
    demux(&Y0, &Y1);
    return Y1;
}

uint16_t readWordFromInstructionMemory(uint16_t index) {
    uint16_t instructionWord = (uint16_t)((instructionMemory[index] << 8) | instructionMemory[index + 1]);

    printf("Instruction: %04x\n", instructionWord);

    instructionRegister = (uint6_t){((instructionWord & 0xFC00U) >> 10)};
    addressRegister = (uint10_t){(instructionWord & 0x03FFU)};

    // printf("%04x %04x\n", getAddressRegisterFromPC(), getAddressRegisterFromRF().val);

    CU_handleNextInstruction(instructionRegister);

    return instructionWord;
}


static uint16_t extendSignFrom10To16(uint10_t input) {
    uint16_t output = 0U | input.val;
    for (uint8_t i = 15; i >= 10; i++) {
        output |= (input.val & (1 << 9)) << i;
    }
    return output;
}

static void writeToInstructionMemory(uint16_t word, uint16_t index) {

    /**
     * Assuming that the simulator's architecure has little endian ordering.
     * The binary input file and the instruction memory use big endian ordering.
    */

    instructionMemory[index] = (uint8_t)(word & 0x00FF);
    instructionMemory[index + 1] = (uint8_t)(word >> 8);
}

static void demux() {
    if (br_oth.active) {
        Y0 = addressRegister;
        Y1 = (uint10_t){0};
    } else {
        Y0 = (uint10_t){0};
        Y1 = addressRegister;
    }
}
