MOV Y, #5
STR Y, 5
LDR X, 5
l1: SUB X, #1
BRZ end
BRA l1
end: ADD X, #100
PSH X
ADD X, #10
POP X
ADD Y, X
MOV X, Y
SUB X, #94
MOV Y, X
MOV Y, #5
JMP a
MUL Y, Y
LDR X, 50
HLT
a: ADD Y, #10
STR Y, 50
RET