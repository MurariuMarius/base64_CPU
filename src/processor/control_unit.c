#include <stdio.h>

#include "internals.h"
#include "opcodes.h"


signal next;
signal bra;
signal jmp;
signal branch_other;
signal end;
uint2_t spOp;


// Test
static void mock_PSH_op() {
    static uint16_t i = 10U;
    i++;
    printf("PUSH %d\n", i);
    store(i, SP_operation());
}

// Test
static void mock_POP_op() {
    printf("POP %d\n", load(SP_operation()));
}

void CU_handleNextInstruction(uint6_t opCode) {
    switch (opCode.val) {
    case HLT:
        end = ACTIVE;
        break;

    // TODO

    case PSH:
        spOp.val = 1U;
        mock_PSH_op();
        break;

    case POP:
        spOp.val = 2U;
        mock_POP_op();
        break;

    default:
        break;
    }
}