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

void request(signal IO_type) {
    if (!IO_type.active) {
        send = ACTIVE;
    }
}

void read() {
    uint16_t buffer;
    
    int wordsRead = fread(&buffer, sizeof(uint8_t), 2, in);

    // Convert to big endian ordering
    // (assuming that the machine running the program has little endinan ordering)
    if (wordsRead == 1) {
        IO_data = (0xFF & buffer) << 8;
    } else {
        IO_data = buffer >> 8;
        IO_data |= (0xFF & buffer) << 8;
    }

    printf("DRVR: Read %d byte(s)\n", wordsRead);
    if (wordsRead == 0) {
        send = INACTIVE;
    }
}

void write() {
    // Convert to big endian ordering
    // (assuming that the machine running the program has little endinan ordering)
    uint16_t buffer = IO_data >> 8;
    buffer |= (0xFF & IO_data) << 8;

    fwrite(&buffer, sizeof(uint16_t), 1, out);
}
