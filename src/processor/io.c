#include <stdio.h>

#include "internals.h"

uint16_t IO_data;
signal type;

#define IN INACTIVE
#define OUT ACTIVE

void input() {
    type = IN;
    uint16_t requestedWords = sign_extend_9_to_16_bits();
    uint16_t readWords = 0;
    request(type);
    while (readWords < requestedWords) {
        read();
        if (!send.active) break;
        printf("IO: Read %04x\n", IO_data);
        store(IO_data, readWords++);
    }
    store(readWords, requestedWords);
}