import sys
from parsers import AssemblyFileParser

if __name__ == "__main__":
    if (len(sys.argv) != 2):
        print("Usage: python3 main.py <asm file>")
        sys.exit(1)

    fileParser = AssemblyFileParser(sys.argv[1])
    fileParser.parse()
    fileParser.write('out.bin')
    