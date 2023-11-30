#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "opcodes.h"
#include "internals.h"

int main(int argc, char **argv) {

    if (argc != 2) {
        printf("Usage: ./simulate <bin_file>\n");
        exit(1);
    }

    initializeInstructionMemory(argv[1]);

    int PC = 0;

    uint16_t instruction;
    while((instruction = readWordFromInstructionMemory(PC)) != 0) {
        printf("%04x\n", instruction);
        PC += 2;
    }
}
