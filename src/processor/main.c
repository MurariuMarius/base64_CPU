#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "opcodes.h"
#include "internals.h"

FILE *in, *out;
signal send;

int main(int argc, char **argv) {

    if (argc < 2 || argc > 4) {
        printf("Usage: ./simulate <bin_file> [in_file] [out_file]\n");
        exit(1);
    }

    if (argc >= 3) {
        in = fopen(argv[2], "rb");
    }

    if (argc == 4) {
        out = fopen(argv[3], "wb");
    }

    initializeInstructionMemory(argv[1]);

    next = ACTIVE;
    
    // Driver
    while(1) {
       nextPC();
       if (end.active) break;
    }

    fclose(in);
    fclose(out);
}

void request(signal type) {
    if (!type.active) {
        send = ACTIVE;
    }
}

void read() {
    uint16_t buffer;
    
    int wordsRead = fread(&buffer, sizeof(uint16_t), 1, in);

    // Convert to big endian ordering
    IO_data = buffer >> 8;
    IO_data |= (0xFF & buffer) << 8;

    printf("DRVR: Read %d word(s)\n", wordsRead);
    if (wordsRead == 0) {
        send = INACTIVE;
    }
}
