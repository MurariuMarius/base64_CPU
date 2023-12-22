memoryInstructions = {
    "LDR": 0b000001,
    "STR": 0b000010
}

indirectMemoryInstructions = {
    "LDR_RI": 0b010100,
    "STR_RI": 0b010101
}

branchInstructions = {
    "BRZ": 0b000100,
    "BRN": 0b000101,
    "BRC": 0b000110,
    "BRO": 0b000111,
    "BRA": 0b001000,
    "JMS": 0b010000,
    "RET": 0b010001,
}

stackInstructions = {
    "PSH": 0b010010,
    "POP": 0b010011
}

ALUInstructions = {
    "ADD": 0b011110,
    "SUB": 0b011111,
    "CMP": 0b100000,
    "TST": 0b100001,
    "LSR": 0b100010,
    "LSL": 0b100011,
    "SAR": 0b100100,
    "SAL": 0b100101,
    "MOV": 0b100110,
    "MUL": 0b100111,
    "DIV": 0b101000,
    "MOD": 0b101001,
    "AND": 0b101010,
    "OR":  0b101011,
    "XOR": 0b101100,
    "NOT": 0b101101,
    "INC": -1,
    "DEC": -1
}

immediateALUInstructions = {
    "ADDI": 0b101110,
    "SUBI": 0b101111,
    "CMPI": 0b110000,
    "TSTI": 0b110001,
    "LSRI": 0b110010,
    "LSLI": 0b110011,
    "SARI": 0b110100,
    "SALI": 0b110101,
    "MOVI": 0b110110,
    "MULI": 0b110111,
    "DIVI": 0b111000,
    "MODI": 0b111001,
    "ANDI": 0b111010,
    "ORI":  0b111011,
    "XORI": 0b111100,
    "NOTI": 0b111101,
    "INCI": 0b111110,
    "DECI": 0b111111
}

HLT = {
    "HLT": 0b000000
}