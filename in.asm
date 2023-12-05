MOV Y, #5
STR Y, 5
LDR X, 5
l1: SUB X, #1
BRZ end
BRA l1
end: PSH X
PSH X
POP X
POP X
HLT