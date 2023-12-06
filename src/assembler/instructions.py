import re

from exceptions import InvalidInstructionException
from header import *
from opcodes import *

class Instruction:
    def __init__(self, opcode):
        self.opcode = opcode
    
    def __repr__(self) -> str:
        return '\n{0:b}|'.format(self.opcode)

class BranchInstruction(Instruction):
    def __init__(self, instruction, labels):
        opcode, address = self.__validate(instruction, labels)

        super().__init__(branchInstructions.get(opcode.upper()))
        self.address = address
    
    def __validate(self, instruction, labels):
        if len(instruction) == 1 and instruction[0] == 'ret':
            return instruction[0], 0

        if len(instruction) != 2:
            raise InvalidInstructionException(instruction)
        branchAddress = labels.get(instruction[1])
        if branchAddress is None:
            raise InvalidInstructionException(instruction)
        return instruction[0], branchAddress
    
    def __repr__(self) -> str:
        return super().__repr__() + f'{self.address}'
    
    def getBytes(self):
        result = (self.opcode << 10) | (0x3FF & self.address)
        return bytearray(result.to_bytes(2))
    

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
    
    def getBytes(self):
        result = (self.opcode << 10) | ((1 & self.registerAddress) << 9) | (0x1FF & self.immediate)
        return bytearray(result.to_bytes(2))
    

class ALUInstruction(Instruction):
    def __init__(self, instruction):
        opcode, registerAddress, operandRegisterAddress = self.__validate(instruction)

        super().__init__(ALUInstructions.get(opcode.upper()))
        self.registerAddress = registerAddress
        self.operandRegisterAddress = operandRegisterAddress

    def __validate(self, instruction):
        if len(instruction) != 3 or \
            instruction[1].upper() not in REGISTERS or \
            instruction[2].upper() not in REGISTERS:
            raise InvalidInstructionException(instruction)
        return instruction[0], REGISTERS[instruction[1].upper()], REGISTERS[instruction[2].upper()]

    def __repr__(self) -> str:
        return super().__repr__() + f"{self.registerAddress}|{self.operandRegisterAddress}"
    
    def getBytes(self):
        result = (self.opcode << 10) | ((1 & self.registerAddress) << 9) | (0x1 & self.operandRegisterAddress)
        return bytearray(result.to_bytes(2))
    
    def get(instruction):
        if "#" in instruction[2]:
            return ImmediateALUInstruction(instruction)
        return ALUInstruction(instruction)
    

class ImmediateALUInstruction(Instruction):
    def __init__(self, instruction):
        opcode, registerAddress, immediate = self.__validate(instruction)

        super().__init__(immediateALUInstructions.get(opcode.upper()))
        self.registerAddress = registerAddress
        self.immediate = immediate

    def __validate(self, instruction):
        if len(instruction) != 3 or \
            instruction[1].upper() not in REGISTERS or \
            not re.match("^#[0-9]*$", str(instruction[2])):
            raise InvalidInstructionException(instruction)
        return instruction[0]+"I", REGISTERS[instruction[1].upper()], int(instruction[2].strip("#"))

    def __repr__(self) -> str:
        return super().__repr__() + f"{self.registerAddress}|{self.immediate}"
    
    def getBytes(self):
        result = (self.opcode << 10) | ((1 & self.registerAddress) << 9) | (0x1FF & self.immediate)
        return bytearray(result.to_bytes(2))
    

class StackInstruction(Instruction):
    def __init__(self, instruction):
        opcode, registerAddress = self.__validate(instruction)

        super().__init__(stackInstructions.get(opcode.upper()))
        self.registerAddress = registerAddress
    
    def __validate(self, instruction):
        if len(instruction) != 2 or \
            instruction[1].upper() not in REGISTERS:
            raise InvalidInstructionException(instruction)
        return instruction[0], REGISTERS[instruction[1].upper()]
        
    def __repr__(self) -> str:
        return super().__repr__() + f"{self.registerAddress}"
    
    def getBytes(self):
        result = (self.opcode << 10) | ((1 & self.registerAddress) << 9)
        return bytearray(result.to_bytes(2))


class HaltInstruction(Instruction):
    def __init__(self):
        super().__init__(HLT["HLT"])

    def getBytes(self):
        return bytearray((self.opcode << 10).to_bytes(2))