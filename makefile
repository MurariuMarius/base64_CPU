SIM_PATH = src/processor
ASSM_PATH = src/assembler

compile:
	gcc -Wall ${SIM_PATH}/main.c \
	 ${SIM_PATH}/ALU_flags.c \
	 ${SIM_PATH}/control_unit.c \
	 ${SIM_PATH}/data_memory.c \
	 ${SIM_PATH}/encoder.c \
	 ${SIM_PATH}/instruction_memory.c \
	 ${SIM_PATH}/io.c \
	 ${SIM_PATH}/program_counter.c \
	 ${SIM_PATH}/register_file.c \
	 ${SIM_PATH}/stack_pointer.c \
	 ${SIM_PATH}/uprogram_sequencer.c \
	 -o simulate

run: compile
	./simulate out.bin in.txt sim_out.bin

assemble:
	python3 ${ASSM_PATH}/main.py in.asm

simulate: assemble run
