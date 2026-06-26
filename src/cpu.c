#include <stdio.h>
#include <inttypes.h>
#include <assert.h>
#include "cpu.h"
#include "ISA.h"

uint32_t resolve_addr(uint8_t seg, uint16_t offset) {
    return ((uint32_t)seg<<16) | offset;
}

uint16_t fetch(Machine *machine) {
    uint32_t addr = resolve_addr(machine->cpu.seg, machine->cpu.pc);
    uint16_t high = machine->memory[addr];
    uint16_t low = machine->memory[addr +1];

    machine->cpu.pc += 2;

    return (high << 8) | low;   // basically add the high and low bit to get the actual data, using Big Endian logic
}

void cycle(Machine *machine) {
    while (!machine->cpu.halt) {
        throttle(CLOCK_SPEED);
        uint16_t inst   = fetch(machine);
        uint8_t opcode  = (inst >> 9) & 0x7F;
        uint8_t rd      = (inst >> 6) & 0x07;
        uint8_t r1      = (inst >> 3) & 0x07;
        uint8_t r2      = (inst >> 0) & 0x07;

        // uint32_t pc = resolve_addr(machine->cpu.seg, machine->cpu.pc);
        // uint32_t ld = resolve_addr(machine->cpu.seg, machine->cpu.load_addr);
        // uint32_t sp = resolve_addr(0XFF, machine->cpu.sp);
        // printf(
        //     "\n\nThis is in Cycle\n"
        //     "Program Counter: 0X%06" PRIX32 
        //     "\nCurrent Address: 0X%06" PRIX32 
        //     "\nStack Pointer: 0X%06" PRIX32 
        //     "\nZero Flag: %08b\n",
        //     pc, ld, sp, machine->cpu.flag
        // );

        // printf("opcode 0x%02" PRIx8 "\nDestination Register 0x%02" PRIx8 "\nRegister 1 0x%03" PRIx8 "\nRegister 2 0x%03" PRIx8 "\n", opcode, rd, r1, r2);

        switch (opcode) {
            case MOV:
                printf("MOV REG %d, REG %d\n", rd, r1);
                machine->cpu.r[rd] = machine->cpu.r[r1];
                printf("Register [%d] = 0x%04" PRIX16 "\t%d\n", rd,machine->cpu.r[rd], machine->cpu.r[rd]);
                break;

            case MOVI:
                uint8_t value = (inst >> 0) & 0x3F;
                printf("MOVI REG %d, %d\n", rd, value);
                machine->cpu.r[rd] = value;
                printf("Register [%d] = 0x%04" PRIX16 "\t%d\n", rd,machine->cpu.r[rd], machine->cpu.r[rd]);
                break;

            case HALT:
                printf("HALT\n\n");
                machine->cpu.halt = 0x01;
                break;

            case ADD:
                machine->cpu.r[rd] = machine->cpu.r[r1] + machine->cpu.r[r2];
                printf("ADD REG %d, REG %d, REG %d\n", rd, r1, r2);
                printf("Register [%d] = %d \t 0x%04" PRIX16 "\n", rd, machine->cpu.r[rd], machine->cpu.r[rd]);
                break;
	
	case PUSH:
		machine->cpu.sp -= 2;
		uint32_t addr = resolve_addr(0xFF, machine->cpu.sp);
		machine->memory[addr]		= machine->cpu.r[rd] >> 8;
		machine->memory[addr + 1]	= machine->cpu.r[rd] & 0xFF;
		printf("Added Reg[%d] data to stack\n", rd);
		printf("PUSH Reg %d\n", rd);
		break;
	
	case POP:
		addr	= resolve_addr(0xFF, machine->cpu.sp);
		machine->cpu.r[rd] = (machine->memory[addr] << 8) | machine->memory[addr + 1];
		printf("Popped value %d data from stack\n", machine->cpu.r[rd]);
		printf("POP Reg %d\n", rd);
		machine->cpu.sp += 2;
		// Will add a checking mechanism to avoid overflowing from 0xFFFFFF
		break;


            default:
                printf("You didn't put a valid opcode\n");
                machine->cpu.halt = 1;
                break;
        }
    }
}

uint16_t instruction_maker(uint8_t opcode, uint8_t rd, uint8_t r1, uint8_t r2) {
    assert(r1 <= 7 && "Register 1 can't be above 7");    // There are only 8 registers R0-R7, so it can't pass that value
    assert(r2 <= 7 && "Register 2 can't be above 7");
    assert(rd <= 7 && "Register destination can't be above 7");
    uint16_t inst = (opcode << 9) | (rd << 6) | (r1 << 3) | (r2 << 0);
    return inst;
}

void add_to_memory(uint16_t inst, Machine *machine) {
    uint8_t high = inst >> 8;
    uint8_t low = inst;

    uint32_t addr = resolve_addr(machine->cpu.seg, machine->cpu.load_addr);
    machine->memory[addr] = high;
    machine->memory[addr + 1] = low;
    machine->cpu.load_addr += 2;
}

void start_machine(Machine *machine) {
    machine->cpu.flag       = 0x00;
    machine->cpu.seg        = 0X01;     // Setting the segment to start of RAM

    machine->cpu.pc         = 0x1000;
    machine->cpu.load_addr  = 0x1000;

    machine->cpu.sp         = 0xFFFF;   //Setting the stack pointer
}

void throttle(double hz) {
    double seconds_per_cycle = 1 / hz;
    struct timespec ts;
    ts.tv_sec = (time_t)seconds_per_cycle;
    ts.tv_nsec = (long)((seconds_per_cycle - ts.tv_sec) * 1e9);
    nanosleep(&ts, NULL);
}
