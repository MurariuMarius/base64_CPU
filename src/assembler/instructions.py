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

        super().__init__(branchInstructions.get(opcode))
        self.address = address
    
    def __validate(self, instruction, labels):
        if len(instruction) == 1 and instruction[0] == 'RET':
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

        super().__init__(memoryInstructions.get(opcode))
        self.registerAddress = registerAddress
        self.immediate = immediate
    
    def __validate(self, instruction):
        if len(instruction) != 3 or \
            instruction[1] not in REGISTERS or \
            not instruction[2].isdigit() or \
            int(instruction[2]) > MEMORY_SIZE:
            raise InvalidInstructionException(instruction)
        return instruction[0], REGISTERS[instruction[1]], int(instruction[2])

    def __repr__(self) -> str:
        return super().__repr__() + f'{self.registerAddress}|{self.immediate}'
    
    def getBytes(self):
        result = (self.opcode << 10) | ((1 & self.registerAddress) << 9) | (0x1FF & self.immediate)
        return bytearray(result.to_bytes(2))
    
    def get(instruction):
        if len(instruction) == 3 and instruction[2].startswith('[') and instruction[2].endswith(']'):
            return RegisterIndirectMemoryInstruction(instruction)
        return MemoryInstruction(instruction)
    

class RegisterIndirectMemoryInstruction(Instruction):
    def __init__(self, instruction):
        opcode, registerAddress, indirectRegister, offset = self.__validate(instruction)

        super().__init__(indirectMemoryInstructions.get(opcode + "_RI"))
        self.registerAddress = registerAddress
        self.indirectRegister = indirectRegister
        self.offset = offset
    
    def __validate(self, instruction):
        def getRegister(operand):
            if match := re.match('^(SP)|[A-Z]', operand):
                register = match.group()
                if register == 'SP':
                    return 0x3
                if register in REGISTERS:
                    return REGISTERS[register]
            raise InvalidInstructionException(instruction)

        def getOffset(operand):
            if match := re.search('[\+-][0-9]{1,3}', operand):
                operand = match.group()
                return int(operand) % 0x80  # complement if negative (7 bit field)
            return 0


        if len(instruction) != 3 or \
            instruction[1] not in REGISTERS or \
            not re.match('^\[((SP)|[A-Z])([\+-][0-9]{1,3})?\]$', instruction[2]):
            raise InvalidInstructionException(instruction)
        
        instruction[2] = instruction[2].strip('[]')
        return instruction[0], REGISTERS[instruction[1]], getRegister(instruction[2]), getOffset(instruction[2])

    def __repr__(self) -> str:
        return super().__repr__() + f'{self.registerAddress}|{self.indirectRegister}|{self.offset}'
    
    def getBytes(self):
        result = (self.opcode << 10) | ((1 & self.registerAddress) << 9) | ((0x3 & self.indirectRegister) << 7) | (0x7F & self.offset)
        return bytearray(result.to_bytes(2))
    

class ALUInstruction(Instruction):
    def __init__(self, instruction):
        opcode, registerAddress, operandRegisterAddress = self.__validate(instruction)

        super().__init__(ALUInstructions.get(opcode))
        self.registerAddress = registerAddress
        self.operandRegisterAddress = operandRegisterAddress

    def __validate(self, instruction):
        if len(instruction) != 3 or \
            instruction[1] not in REGISTERS or \
            instruction[2] not in REGISTERS:
            raise InvalidInstructionException(instruction)
        return instruction[0], REGISTERS[instruction[1]], REGISTERS[instruction[2]]

    def __repr__(self) -> str:
        return super().__repr__() + f"{self.registerAddress}|{self.operandRegisterAddress}"
    
    def getBytes(self):
        result = (self.opcode << 10) | ((1 & self.registerAddress) << 9) | ((0x1 & self.operandRegisterAddress) << 7)
        return bytearray(result.to_bytes(2))
    
    def get(instruction):
        if len(instruction) == 2 and instruction[0] in ["DEC", "INC"]:
            instruction.append("#0")

        if "#" in instruction[2]:
            return ImmediateALUInstruction(instruction)
        return ALUInstruction(instruction)
    

class ImmediateALUInstruction(Instruction):
    def __init__(self, instruction):
        opcode, registerAddress, immediate = self.__validate(instruction)

        super().__init__(immediateALUInstructions.get(opcode))
        self.registerAddress = registerAddress
        self.immediate = immediate

    def __validate(self, instruction):
        if len(instruction) != 3 or \
            instruction[1] not in REGISTERS or \
            not re.match("^#-?[0-9]*$", str(instruction[2])):
            raise InvalidInstructionException(instruction)
        return instruction[0]+"I", REGISTERS[instruction[1]], int(instruction[2].strip("#"))

    def __repr__(self) -> str:
        return super().__repr__() + f"{self.registerAddress}|{self.immediate}"
    
    def getBytes(self):
        result = (self.opcode << 10) | ((1 & self.registerAddress) << 9) | (0x1FF & self.immediate)
        return bytearray(result.to_bytes(2))
    

class StackInstruction(Instruction):
    def __init__(self, instruction):
        opcode, registerAddress = self.__validate(instruction)

        super().__init__(stackInstructions.get(opcode))
        self.registerAddress = registerAddress
    
    def __validate(self, instruction):
        if len(instruction) != 2 or \
            instruction[1] not in REGISTERS:
            raise InvalidInstructionException(instruction)
        return instruction[0], REGISTERS[instruction[1]]
        
    def __repr__(self) -> str:
        return super().__repr__() + f"{self.registerAddress}"
    
    def getBytes(self):
        result = (self.opcode << 10) | ((1 & self.registerAddress) << 9)
        return bytearray(result.to_bytes(2))


class IOInstruction(Instruction):
    def __init__(self, instruction):
        opcode, value = self.__validate(instruction)

        super().__init__(IOInstructions.get(opcode))
        self.value = value

    def __validate(self, instruction):
        if len(instruction) != 2 or \
            not instruction[1].isdigit() or \
            int(instruction[1]) > MEMORY_SIZE or int(instruction[1]) < 0:
            raise InvalidInstructionException(instruction)
        return instruction[0], int(instruction[1])

    def __repr__(self) -> str:
        return super().__repr__() + f"{self.value}"
    
    def getBytes(self):
        result = (self.opcode << 10) | (0x3FF & self.value)
        return bytearray(result.to_bytes(2))


class EncodeInstruction(Instruction):
    def __init__(self):
        super().__init__(ENC["ENC"])

    def getBytes(self):
        return bytearray((self.opcode << 10).to_bytes(2))


class HaltInstruction(Instruction):
    def __init__(self):
        super().__init__(HLT["HLT"])

    def getBytes(self):
        return bytearray((self.opcode << 10).to_bytes(2))