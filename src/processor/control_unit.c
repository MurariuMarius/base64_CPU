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
signal str;


static void resetFlowControlSignals() {
    next = INACTIVE;
    br_always = INACTIVE;
    br_oth = INACTIVE;
}

static void resetSignals() {
    lse = INACTIVE;
    aluINSTR = INACTIVE;
    str = INACTIVE;
    ldm = INACTIVE;
}

void CU_handleNextInstruction() {
    resetFlowControlSignals();
    
    switch(instruction.val){
        case MOV:
            lse = ACTIVE;
            register_file();
            break;
        case LDR:
            ldm = ACTIVE;
            register_file();
            break;
        case STR:
            str = ACTIVE;
            register_file();
            break;
        case PSH:
            stackOP.val = 0b01;
            str = ACTIVE;
            register_file();
            break;
        case POP: 
            stackOP.val = 0b10;
            ldm = ACTIVE;
            register_file();
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
