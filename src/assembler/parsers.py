import re

from exceptions import LabelAlreadyDefinedException, InvalidOpcodeException
from instructions import BranchInstruction, MemoryInstruction, ALUInstruction, HaltInstruction, StackInstruction, IOInstruction
import opcodes

class AssemblyFileParser:
    def __init__(self, assemblyFileName):
        self.assemblyFileName = assemblyFileName
        self.labels = {}
        self.instructions = []
        self.lineParser = LineParser()

    def parse(self):
        with open(self.assemblyFileName, "r") as assemblyFile:
            for line in assemblyFile:
                line = self.lineParser.extract(line.strip().upper())
                if line:
                    self.instructions.append(line)
            

        self.__getLabels()

        print(self.instructions)
        print(self.labels)

        for i in range(len(self.instructions)):
            self.instructions[i] = self.lineParser.getInstruction(self.instructions[i], self.labels)

        print(self.instructions)


    def write(self, outputFile):
        with open(outputFile, "wb") as outputFile:
            for instruction in self.instructions:
                outputFile.write(instruction.getBytes())

    def __getLabels(self):
        for i in range(len(self.instructions)):
            label = self.lineParser.getLabel(self.instructions[i])
            if label:
                if (label := label.strip(":")) in self.labels:
                    raise LabelAlreadyDefinedException(label)
                self.labels[label] = 2 * i
                self.instructions[i][0] = label


class LineParser:
    def __init__(self):
        self.labelPattern = "^[_A-Z0-9]*:"

    def getInstruction(self, line, labels : dict):
        if labels.get(line[0]):
            line = line[1:]

        print(line)

        return InstructionParser.parse(line, labels)

    def getLabel(self, line):
        label = None
        if match := re.match(self.labelPattern, line[0]):
            label = match.group()
        return label

    def extract(self, line : str):
        if match := re.search('\s*;.*$', line):
            print(match.group())
            line = line.removesuffix(match.group())
        line = line.replace(",", "").split(" ")
        return line if line[0] else None
    

class InstructionParser:
    def parse(instruction, labels : dict):
        opcode = instruction[0]
        if opcode in opcodes.branchInstructions:
            instruction = BranchInstruction(instruction, labels)
        elif opcode in opcodes.memoryInstructions:
            instruction = MemoryInstruction.get(instruction)
        elif opcode in opcodes.stackInstructions:
            instruction = StackInstruction(instruction)
        elif opcode in opcodes.ALUInstructions:
            instruction = ALUInstruction.get(instruction)
        elif opcode in opcodes.IOInstructions:
            instruction = IOInstruction(instruction)
        elif opcode in opcodes.HLT:
            instruction = HaltInstruction()
        else:
            raise InvalidOpcodeException(instruction)
        return instruction
    