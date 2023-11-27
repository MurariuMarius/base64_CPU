import re
from exceptions import LabelAlreadyDefinedException

class LineParser:
    def __init__(self):
        self.labels = {}
        self.labelPattern = '^[_a-z0-9]*: '

    def parse(self, line, address):
        return None

    def mapLabel(self, line, address):
        if match := re.match(self.labelPattern, line):
            label = match.group().split(':')[0]
            if (label in self.labels):
                raise LabelAlreadyDefinedException(label, address + 1)
            self.labels[label] = address
