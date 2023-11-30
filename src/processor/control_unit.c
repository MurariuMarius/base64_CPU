#include "internals.h"
#include "opcodes.h"

signal next;
signal bra;
signal jmp;
signal branch_other;
signal end;

void CU_handleNextInstruction(uint6_t opCode) {
    switch (opCode.val) {
    case HLT:
        end = ACTIVE;
        break;

    // TODO

    default:
        break;
    }
}