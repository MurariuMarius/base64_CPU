IN 84
LDR X, 0
MOV Y, #84
SUB Y, X
STR Y, 0
ADD X, #1 ; Reset ZF
ENC
OUT 112
ADD X, #0
HLT
