#include <stdio.h>

#include "internals.h"

uint16_t IO_data;

static uint16_t requestedWords;
static uint16_t wordsProcessed;

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
    while (requestedWords > 0) {
        IO_data = load(requestedWords--);
        printf("IO: Wrote %04x\n", IO_data);
        write();
    }
}

void io() {
    requestedWords = sign_extend_9_to_16_bits();
    requestedWords %= DATA_MEMORY_SIZE;
    wordsProcessed = 0;
    request(IO_type);
    if (IO_type.active) {
        output();
    } else {
        input();
    }
}
