import re

from exceptions import InvalidInstructionException
from header import *
from opcodes import *

class Instruction:
    def __init__(self, opcode):
        self.opcode = opcode
    
    def __repr__(self) -> str:
        # return '\n{0:b}|'.format(self.opcode)
        return f'\n{self.opcode}|'


class BranchInstruction(Instruction):
    def __init__(self, instruction, labels):
        opcode, address = self.__validate(instruction, labels)

        super().__init__(branchInstructions.get(opcode.upper()))
        self.address = address
    
    def __validate(self, instruction, labels):
        if len(instruction) != 2:
            raise InvalidInstructionException(instruction)
        branchAddress = labels.get(instruction[1])
        if branchAddress is None:
            raise InvalidInstructionException(instruction)
        return instruction[0], branchAddress
    
    def __repr__(self) -> str:
        return super().__repr__() + f'{self.address}'
    

class MemoryInstruction(Instruction):
    def __init__(self, instruction):
        opcode, registerAddress, immediate = self.__validate(instruction)

        super().__init__(memoryInstructions.get(opcode.upper()))
        self.registerAddress = registerAddress
        self.immediate = immediate
    
    def __validate(self, instruction):
        if len(instruction) != 3 or \
            instruction[1].upper() not in REGISTERS or \
            not instruction[2].isdigit() or \
            int(instruction[2]) > MEMORY_SIZE:
            raise InvalidInstructionException(instruction)
        return instruction[0], REGISTERS[instruction[1].upper()], int(instruction[2])

    def __repr__(self) -> str:
        return super().__repr__() + f'{self.registerAddress}|{self.immediate}'


class ALUInstruction(Instruction):
    def __init__(self, instruction):
        opcode, registerAddress, immediate = self.__validate(instruction)

        super().__init__(ALUInstructions.get(opcode.upper()))
        self.registerAddress = registerAddress
        self.immediate = immediate

    def __validate(self, instruction):
        if len(instruction) != 3 or \
            instruction[1].upper() not in REGISTERS or \
            not re.match("^#[0-9]*$", str(instruction[2])):
            raise InvalidInstructionException(instruction)
        return instruction[0], REGISTERS[instruction[1].upper()], int(instruction[2].strip("#"))

    def __repr__(self) -> str:
        return super().__repr__() + f"{self.registerAddress}|{self.immediate}"
