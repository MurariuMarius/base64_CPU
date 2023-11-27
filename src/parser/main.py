import sys
from lineparser import LineParser

if __name__ == "__main__":
    if (len(sys.argv) != 2):
        print("Usage: python3 main.py <asm file>")
        sys.exit(1)

    lineParser = LineParser()
    count = 0

    with open(sys.argv[1]) as assemblyFile:
        for line in assemblyFile:
            # lineParser.parse(line.strip().lower())
            lineParser.mapLabel(line.strip().lower(), count)
            count += 1

    print(lineParser.labels)
