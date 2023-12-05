#include <stdio.h>
#include "opCodes.h"
#include "internals.h"

uint2_t stackOP;     
signal aluINSTR;   
signal br_oth;      
signal br_always;         
signal next;        
signal end;
signal jmp;
signal lse;
signal ldm;

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

static void resetFlowControlSignals() {
    next = INACTIVE;
    br_always = INACTIVE;
    br_oth = INACTIVE;
}

static void resetSignals() {
    lse = INACTIVE;
    aluINSTR = INACTIVE;
}

void CU_handleNextInstruction() {
    resetFlowControlSignals();
    
    //verificare semnal pt operatii pe stack
    switch(instruction.val){
        case MOV:
            lse = ACTIVE;
            register_file();
            break;
        case PSH:
            stackOP.val = 0b01;
            mock_PSH_op();
            break;
        case POP: 
            stackOP.val = 0b00;
            mock_POP_op();
            break;
        case HLT:
            end.active = 0b1;
            break;
        default:
            break;
    }

    //verificare semnale de branch
    if(instruction.val >= BRZ && instruction.val <= RET){
        br_oth = ACTIVE;
        checkFlags();
        if(instruction.val == BRA){
            br_always = ACTIVE;
        }
    }

    // printf("CU: br_oth: %d, ABS: %d\n", br_oth.active, br_always.active);

    //
    if(instruction.val >= ADD && instruction.val <= TST && instruction.val != MOV) { 
        aluINSTR = ACTIVE;
        register_file();
    }

    next = ACTIVE;

    resetSignals();
}
