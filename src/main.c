#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>

#define MEMORY_SIZE 64 * 1024       // 64 KiB for RAM, no segments for now

#define INST_SET        // Set a value to a register
#define INST_HALT       // set the halt flag

typedef struct {
    uint16_t r[8];      // Invariant 2- stating the 16 bit R0-R7
    
    uint16_t pc;        // Invariant 1 - A register pointing to the next instruction

    uint8_t seg;        // the 8-bit segment register used when offsetting the 16-bit location

    uint8_t halt;       // Invariant 4 - this is basically a bool flag for halt, 1 means halt 0 means not
} CPU;

typedef struct {
    CPU cpu;
    uint8_t memory[MEMORY_SIZE];        // 8 bit because memory fundamentally is a single byte
} Machine;

Machine machine = {0};

uint16_t fetch(Machine *machine) {
    uint16_t high = machine->memory[machine->cpu.pc];
    uint16_t low = machine->memory[machine->cpu.pc +1];

    machine->cpu.pc += 2;

    return (high << 8) | low;   // basically add the high and low bit to get the actual data, using Big Endian logic
}

int main(void) {
    machine.memory[0] = 0x12;    // high byte
    machine.memory[1] = 0x34;   // low byte

    uint16_t result = fetch(&machine);
    printf("The value is: %" PRIx16 "\n", result);

    return 0;
}