LDR X, 4
STR Y, 6
label1: BRZ addr
label2: BRZ _addr
ADD X, #1;#1
TST X, #6
;CMP X, #6
;addr: drPos
addr: BRZ _addr
_addr: BRZ addr
CMP Y #4
HLT