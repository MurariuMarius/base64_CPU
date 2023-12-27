#include <stdio.h>

#include "internals.h"
#include "opcodes.h"

uint16_t IO_data;

static uint16_t requestedWords;
static uint16_t wordsProcessed;
static signal type;

static void input() {
    while (requestedWords > 0) {
        read();
        if (!send.active) break;
        wordsProcessed++;
        printf("IO: Read %04x\n", IO_data);
        store(IO_data, requestedWords--);
    }
    store(wordsProcessed, 0);
}

static void output() {
    uint16_t minimum = type.active ? *getSelectedRegister() : 0;

    while (requestedWords > minimum && send.active) {
        IO_data = load(requestedWords--);
        printf("IO: Wrote %04x to go %d (%d)\n", IO_data, requestedWords - minimum, requestedWords);
        if (type.active) {
            writeBase64();
        } else {
            write();
        }
    }

    // Termitate transmission for base64 output
    if (type.active) {
        IO_data = 0;
        writeBase64();
    }
}

void io() {
    type = sign_extend_9_to_16_bits() & 1 ? ACTIVE : INACTIVE;
    requestedWords = 0x01FF & getOperandRegister();
    wordsProcessed = 0;
    request();
    switch(getOpcode().val) {
        case IN:
            input();
            break;
        case OUT:
            output();
    }
}
