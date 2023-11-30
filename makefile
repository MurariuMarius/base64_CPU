compile:
	gcc -Wall src/processor/main.c src/processor/instruction_memory.c -o simulate

test: compile
	./simulate out.bin