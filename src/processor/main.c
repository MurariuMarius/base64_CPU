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
        out = fopen(argv[3], "w+");
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
uint8_t paddingOffset;

void request() {
    send = ACTIVE;
    paddingOffset = 0;
}

signal incompleteRead;

void read() {
    uint16_t buffer;

    int wordsRead = fread(&buffer, sizeof(uint8_t), 2, in);

    // Convert to big endian ordering
    // (assuming that the machine running the program has little endinan ordering)
    if (wordsRead == 1) {
        IO_data = (0xFF & buffer) << 8;
        incompleteRead = ACTIVE;
    } else if (wordsRead == 2) {
        IO_data = buffer >> 8;
        IO_data |= (0xFF & buffer) << 8;
        incompleteRead = INACTIVE;
    }
    printf("DRVR: incompleteRead: %d\n", incompleteRead.active);

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

extern signal receivedZero;

void writeBase64() {
    // Convert to big endian ordering
    // (assuming that the machine running the program has little endinan ordering)
    uint16_t buffer = IO_data >> 8;
    buffer |= (0xFF & IO_data) << 8;

    printf("DRVR: Received %x\n", buffer);
    if (buffer == 0 || (buffer >> 8) == 0) {
        if (buffer >> 8 == 0 && buffer & 0xFF) {
            paddingOffset++;
            paddingOffset %= 4;
        }

        if (incompleteRead.active) {
            if (buffer == 0) {
                fseek(out, -2, SEEK_CUR);
                fread(&buffer, sizeof(uint8_t), 2, out);
                
                printf("DRVR: Retrieved %x\n", buffer);

                if ((buffer >> 8 != 0 && buffer & 0xFF)) {
                    if (!receivedZero.active || buffer >> 8 != 0x41) {
                        send = INACTIVE;
                        if (paddingOffset == 2){
                            buffer = 0x3d3d;
                            fwrite(&buffer, sizeof(uint8_t), 2, out);
                        }
                        return;
                    }
                }

                buffer &= 0xFF;
                buffer |= 0x3D00;
                fseek(out, -2, SEEK_CUR);
            } else {
                buffer = 0x3D3D;
            }

            fwrite(&buffer, sizeof(uint8_t), 2, out);
            printf("DRVR: Writing 1 byte %x\n", buffer);

        } else {
            printf("DRVR: Padding %d\n", paddingOffset);

            if (paddingOffset == 2){
                buffer = 0x3d3d;
                fwrite(&buffer, sizeof(uint8_t), 2, out);
            } else if (paddingOffset == 3 && buffer != 0) {
                buffer = 0x3d00 | (buffer & 0xFF);
                fwrite(&buffer, sizeof(uint8_t), 2, out);
            }
        }
        
        send = INACTIVE;

    } else {
        printf("DRVR: Padding %d\n", paddingOffset);

        paddingOffset += 2;
        paddingOffset %= 4;

        fwrite(&buffer, sizeof(uint16_t), 1, out);
        printf("DRVR: Writing 2 bytes %x\n", buffer);
    }
}
