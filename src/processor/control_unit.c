#include <stdio.h>
#include "opCodes.h"
#include "internals.h"

uint2_t stackOP;     
signal aluINSTR;   
signal br_oth;      
signal bra;         
signal ABS;         
signal next;        
signal end;

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


void CU_handleNextInstruction(uint6_t instruction){
    next.active = 0b0;
    
    //verificare semnal pt operatii pe stack
    switch(instruction.val){
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
        br_oth.active = 0b1;
        if(instruction.val == BRA){
            ABS.active = 0b1;
        }else{
            ABS.active = 0b0;
        }
        next.active = 0b0;
    }else{
        br_oth.active = 0b0;
        next.active = 0b1;
    }

    //
    if(instruction.val >= ADDI && instruction.val <= TSTI){ 
        aluINSTR.active = 0b1;
    }
}
