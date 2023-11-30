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

    // Driver
    while(1) {
       next = ACTIVE;
       setPC();
       if (end.active) break;
    }
}
