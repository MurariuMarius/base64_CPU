MOV X, #4
PSH Y   ; preserve the value of Y
PSH X   ; pass X as parameter
JMP func
POP X   ; store in X the return value
POP Y   ; retreive the original value of Y
MOV Y, X
MUL X, X
HLT
func: LDR Y, [SP+1] ; access the function parameter through the stack
STR Y, [Y-1]
ADD Y, #10
STR Y, [SP+1] ; store the return value in the stack
LDR Y, [Y-11]
RET