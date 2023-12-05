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
signal immOp;


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
    immOp = INACTIVE;
}

void CU_handleNextInstruction() {
    resetFlowControlSignals();
    
    switch(instruction.val){
        case HLT:
            end = ACTIVE;
            break;
            
        case MOVI:
            immOp = ACTIVE;
        case MOV:
            lse = ACTIVE;
            register_file();
            break;

        case POP: 
            stackOP.val = 0b10;
        case LDR:
            ldm = ACTIVE;
            register_file();
            break;

        case PSH:
            stackOP.val = 0b01;
        case STR:
            str = ACTIVE;
            register_file();
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

    if(instruction.val >= ADD && instruction.val <= NOT && instruction.val != MOV) { 
        aluINSTR = ACTIVE;
        register_file();
    }

    if(instruction.val >= ADDI && instruction.val <= DECI && instruction.val != MOVI) { 
        immOp = ACTIVE;
        aluINSTR = ACTIVE;
        register_file();
    }

    next = ACTIVE;

    resetSignals();
}
