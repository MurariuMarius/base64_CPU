#include <stdio.h>
#include "opCodes.h"
#include "internals.h"

uint2_t stackOP;     
signal aluOp;   
signal br_oth;      
signal br_always;         
signal next;        
signal end;
signal jmp;
signal lse;
signal ldm;
signal str;
signal jmpS;
signal ret;
signal immOp;
signal IO_type;
signal IO_Op;
signal enc;


static void resetSignals() {
    next = INACTIVE;
    br_always = INACTIVE;
    br_oth = INACTIVE;
    ret = INACTIVE;
    jmpS = INACTIVE;
    stackOP.val = 0;

    lse = INACTIVE;
    aluOp = INACTIVE;
    str = INACTIVE;
    ldm = INACTIVE;
    immOp = INACTIVE;
    IO_Op = INACTIVE;
    enc = INACTIVE;
}

void CU_handleNextInstruction() {
    resetSignals();
    
    switch(instruction.val){
        case HLT:
            end = ACTIVE;
            break;

        case ENC:
            executeMicroprogram();
            break;

        case IN:
            IO_Op = ACTIVE;
            IO_type = IO_IN;
            str = ACTIVE;
            io();
            break;

        case OUT:
            IO_Op = ACTIVE;
            IO_type = IO_OUT;
            io();
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
            immOp = ACTIVE;
        case LDR_RI:
            ldm = ACTIVE;
            register_file();
            break;

        case PSH:
            stackOP.val = 0b01;
        case STR:
            immOp = ACTIVE;
        case STR_RI:
            str = ACTIVE;
            register_file();
            break;

        case RET:
            ret = ACTIVE;
            stackOP.val = 0b10;
            break;

        default:
            break;
    }


    //verificare semnale de branch
    if(instruction.val >= BRZ && instruction.val < RET){
        br_oth = ACTIVE;
        checkFlags();
        if(instruction.val == BRA || instruction.val == JMS){
            br_always = ACTIVE;
            if (instruction.val == JMS) {
                stackOP.val = 0b01;
                jmpS = ACTIVE;
            }
        }
    }

    // printf("CU: br_oth: %d, ABS: %d\n", br_oth.active, br_always.active);

    if(instruction.val >= ADD && instruction.val <= NOT && instruction.val != MOV) { 
        aluOp = ACTIVE;
        register_file();
    }

    if(instruction.val >= ADDI && instruction.val <= DECI && instruction.val != MOVI) { 
        immOp = ACTIVE;
        aluOp = ACTIVE;
        register_file();
    }

    next = ACTIVE;

}
