#include <stdio.h>

#include "internals.h"

uint16_t IO_data;

static uint16_t requestedWords;
static uint16_t wordsProcessed;

static void input() {
    while (wordsProcessed < requestedWords) {
        read();
        if (!send.active) break;
        printf("IO: Read %04x\n", IO_data);
        store(IO_data, wordsProcessed++);
    }
    store(wordsProcessed, requestedWords);
}

static void output() {
    while (wordsProcessed < requestedWords) {
        IO_data = load(wordsProcessed++);
        printf("IO: Wrote %04x\n", IO_data);
        write();
    }
}

void io() {
    requestedWords = sign_extend_9_to_16_bits();
    wordsProcessed = 0;
    request(IO_type);
    if (IO_type.active) {
        output();
    } else {
        input();
    }
}
