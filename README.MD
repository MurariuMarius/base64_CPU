# Base64 Encoding CPU

This program simulates an application-specific instruction set processor (ASIP) that **encodes plaintext into base64**.\
Additionaly, the simulated CPU is fitted with general-purpose functionality such as **memory, branching** and various **ALU operations**.

## The assembler
- translates the applicaton-specific instructions into a binary executable file;
- requires a `txt` or `asm` file as input.

## The simulator
- implemented in C, it simulates the behaviour of the CPU and its components;
- requires a binary file as input.

## How to run
1. To create the binary file:
```
python3 src/assembler/main.py <in.asm>
```
2. To compile and run the simulator:
```
make compile
./simulate <out.bin>
```
3. To run the entire simulation (steps 1 and 2), assuming `in.asm` is the assembly file:
```
make simulate
```
