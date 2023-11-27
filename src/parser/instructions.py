class Instruction:
    def __init__(self, label, address, opcode):
        self.label = label
        self.address = address
        self.opcode = opcode
    
    def __repr__(self) -> str:
        return f'{self.label} at {self.address} : {self.opcode}'
    

class MemoryInstruction(Instruction):
    def __init__(self, label, address, opcode, registerAddress, immediate):
        Instruction(label, address, opcode)
        self.registerAddress = registerAddress
        self.immediate = immediate

    def __repr__(self) -> str:
        return super().__repr__() + f' {self.registerAddress} | {self.immediate}'