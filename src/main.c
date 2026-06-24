#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>

#define MEMORY_SIZE 64 * 1024       // 64 KiB for RAM, no segments for now

#define INST_SET    0x10        // Set a value to a register
#define INST_HALT   0x02        // set the halt flag

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

uint16_t cycle(Machine *machine) {
    while (!machine->cpu.halt) {
        uint16_t inst = fetch(machine);

        uint8_t mode    =   (inst >> 15) & 0x01;
        uint8_t opcode  =   (inst >> 10) & 0x1F;
        uint8_t r1      =   (inst >> 5)  & 0x1F;
        uint8_t r2      =   (inst >> 0)  & 0x1F;

        printf("\n\nmode 0x%02" PRIx8 "\topcode 0x%02" PRIx8 "\tr1 0x%02" PRIx8 "\tr2 0x%02" PRIx8 "\n", mode, opcode, r1, r2);
        switch(opcode) {
            case INST_SET:
                if (mode) {
                    machine->cpu.r[r1] = r2;
                }
                else {
                    printf("This isn't immediate");
                    machine->cpu.r[r1] = machine->cpu.r[r2];
                }
                break;
            case INST_HALT:
                machine->cpu.halt = 0x01;
                break;
        }
    }
}

int main(void) {
    uint8_t mode = 0x01;        // 1 = immediate 0 = register to register
    uint8_t opcode = 0x10;       // 5-bits for instruction opcode
    uint8_t r1 = 0x02;
    uint8_t r2 = 0x03;

    uint16_t inst = (mode << 15) | (opcode << 10) | (r1 << 5) | (r2 << 0);

    uint8_t high = inst >> 8;
    uint8_t low = inst;

    machine.memory[0] = high;   // high byte
    machine.memory[1] = low;    // low byte

    mode = 0x00;
    opcode = INST_HALT;

    inst ^= inst;
    inst = (mode << 15) | (opcode << 10);

    high = inst >> 8;
    low = inst;
    machine.memory[2] = high;
    machine.memory[3] = low;

    cycle(&machine);
    printf("\n\nRegister 2 = %d", machine.cpu.r[2]);        // Must be a value of 3
    return 0;
}