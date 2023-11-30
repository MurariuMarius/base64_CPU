compile:
	gcc -Wall src/processor/main.c src/processor/instruction_memory.c src/processor/program_counter.c src/processor/control_unit.c -o simulate

test: compile
	./simulate out.bin