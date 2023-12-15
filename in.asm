MOV X, #15
PSH Y
PSH X
JMP fib
POP X
POP Y
ADD X, #0
HLT
fib: LDR X, [SP+1] ; acces argument prin stiva

CMP X, #2 ; if X <= 1
BRN end ; caz de baza

rec: MOV Y, #0 ; initializare variabila pentru suma

DEC X ; preagatire X - 1

PSH X ; pastreaza X
PSH X
JMP fib
POP Y ; Y = fib(X - 1)
POP X

DEC X ; pregatire X - 2

PSH Y ; pastreaza Y
PSH X
JMP fib
POP X  ; X = fib(X - 2)
POP Y

ADD X, Y ; X = fib(X - 1) + fib(X - 2)
end: STR X, [SP+1] ; retureaza X prin stiva
RET