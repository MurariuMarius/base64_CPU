import re

from exceptions import LabelAlreadyDefinedException, InvalidInstructionException
from instructions import BranchInstruction, MemoryInstruction, ALUInstruction
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
                line = self.lineParser.extract(line.strip().lower())
                if line:
                    self.instructions.append(line)
            

        self.__getLabels()

        print(self.instructions)
        print(self.labels)

        for i in range(len(self.instructions)):
            self.instructions[i] = self.lineParser.getInstruction(self.instructions[i], self.labels)

        print(self.instructions)


    def write(self, outputFile):
        with open(outputFile, "w") as outputFile:
            for instruction in self.instructions:
                outputFile.write(repr(instruction))

    def __getLabels(self):
        for i in range(len(self.instructions)):
            label = self.lineParser.getLabel(self.instructions[i])
            if label:
                if label in self.labels:
                    raise LabelAlreadyDefinedException(label.strip(":"))
                self.labels[label.strip(":")] = i
                self.instructions[i][0] = label.strip(":")


class LineParser:
    def __init__(self):
        self.labelPattern = "^[_a-z0-9]*:"

    def getInstruction(self, line, labels : dict):
        if labels.get(line[0].strip(":")):
            line = line[1:]

        print(line)

        return InstructionParser.parse(line, labels)

    def getLabel(self, line):
        label = None
        if match := re.match(self.labelPattern, line[0]):
            label = match.group()
        return label

    def extract(self, line : str):
        line = line.split(";", 1)[0]
        line = line.replace(",", "").split(" ")
        
        return line if line[0] else None
    

class InstructionParser:
    def parse(instruction, labels : dict):
        opcode = instruction[0]
        if opcode.upper() in opcodes.branchInstructions:
            instruction = BranchInstruction(instruction, labels)
        elif opcode.upper() in opcodes.memoryInstructions:
            instruction = MemoryInstruction(instruction)
        else:
            pass
            # raise InvalidInstructionException(instruction)
        return instruction