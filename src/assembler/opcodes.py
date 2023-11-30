memoryInstructions = {
    "LDR": 0b000001,
    "STR": 0b000010
}

branchInstructions = {
    "BRZ": 0b000100,
    "BRN": 0b000101,
    "BRC": 0b000110,
    "BRO": 0b000111,
    "BRA": 0b001000,
    "JMP": 0b010000,
    "RET": 0b010001,
}

ALUInstructions = {
    "ADD": 0b100000,
    "SUB": 0b100001,
    "LSR": 0b100010,
    "LSL": 0b100011,
    "RSR": 0b100100,
    "RSL": 0b100101,
    "MOV": 0b100110,
    "MUL": 0b100111,
    "DIV": 0b101000,
    "MOD": 0b101001,
    "AND": 0b101010,
    "OR":  0b101011,
    "XOR": 0b101100,
    "NOT": 0b101101,
    "CMP": 0b101110,
    "TST": 0b101111,
    "INC": 0b110000,
    "DEC": 0b110001
}

HLT = {
    "HLT": 0b000000
}