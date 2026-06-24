#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include <assert.h>

#define MEMORY_SIZE 64 * 1024       // 64 KiB for RAM, no segments for now

#define INST_SET    0x10        // Set a value to a register
#define INST_HALT   0x02        // set the halt flag
#define INST_ADD    0x03
// For mode 0 is register-to-register and 1 is immediate

typedef struct {
    uint16_t r[8];      // Invariant 2- stating the 16 bit R0-R7
    
    uint16_t pc;        // Invariant 1 - A register pointing to the next instruction
    uint16_t load_addr;

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

void cycle(Machine *machine) {
    while (!machine->cpu.halt) {
        uint16_t inst = fetch(machine);
        uint8_t mode   = (inst >> 15) & 0x01;
        uint8_t opcode = (inst >> 10) & 0x1F;
        uint8_t r1     = (inst >> 5)  & 0x1F;
        uint8_t r2     = (inst >> 0)  & 0x1F;

        printf("\n\nmode 0x%02" PRIx8 "\topcode 0x%02" PRIx8 "\tr1 0x%02" PRIx8 "\t\tr2 0x%02" PRIx8 "\n",
               mode, opcode, r1, r2);

        switch (opcode) {
            case INST_SET:
                if (mode) {
                    machine->cpu.r[r1] = r2;
                    printf("Register %d = %d\n\n", r1, r2);
                } else {
                    printf("Register %d = Register %d with value %d\n\n", r1, r2, machine->cpu.r[r2]);
                    machine->cpu.r[r1] = machine->cpu.r[r2];
                }
                break;

            case INST_HALT:
                machine->cpu.halt = 0x01;
                break;

            case INST_ADD:
                if (mode) {
                    printf("Register %d was %d\n", r1, machine->cpu.r[r1]);
                    machine->cpu.r[r1] += r2;
                    printf("Now Register %d is %d\n", r1, machine->cpu.r[r1]);
                } else {
                    printf("Register %d was %d\n", r1, machine->cpu.r[r1]);
                    printf("Register %d was %d\n", r2, machine->cpu.r[r2]);
                    machine->cpu.r[r1] += machine->cpu.r[r2];
                    printf("Now Register %d is %d\n", r1, machine->cpu.r[r1]);
                }
                break;

            default:
                printf("You didn't put a valid opcode\n");
                machine->cpu.halt = 1;
                break;
        }
    }
}
uint16_t instruction_maker(uint8_t mode, uint8_t opcode, uint8_t r1, uint8_t r2) {
    assert(r1 <= 7 && "Register 1 can't be above 7");    // There are only 8 registers R0-R7, so it can't pass that value
    if (!mode) assert(r2 <= 7 && "Register 2 can't be above 7");
    else assert(r2 <= 31 && "Immediate value can't be above 31");
    uint16_t inst = (mode << 15) | (opcode << 10) | (r1 << 5) | (r2 << 0);
    return inst;
}

void add_to_memory(uint16_t inst, Machine *machine) {
    uint8_t high = inst >> 8;
    uint8_t low = inst;
    machine->memory[machine->cpu.load_addr] = high;
    machine->memory[machine->cpu.load_addr + 1] = low;
    machine->cpu.load_addr += 2;
}

int main(void) {
    uint16_t inst = instruction_maker(0x01, INST_SET, 0x02, 0x08);  //r2 = 8
    add_to_memory(inst, &machine);
    printf("Passed first\n");

    inst = instruction_maker(0x01, INST_ADD, 0x02, 0x1F);       // r2 = 8 + 31 = 39
    add_to_memory(inst, &machine);
    printf("Passed second\n");
    
    inst = instruction_maker(0x01, INST_SET, 0x01, 0x08);       // r1 = 8
    add_to_memory(inst, &machine);
    printf("Passed third\n");
    
    inst = instruction_maker(0x00, INST_ADD, 0x02, 0x01);   // r2 = r1 + r2 = 8 + 39 = 47
    add_to_memory(inst, &machine);
    printf("Passed fourth\n");

    inst = instruction_maker(0x00, INST_HALT, 0x00, 0x00);
    add_to_memory(inst, &machine);
    printf("Passed fifth\n");

    cycle(&machine);
    printf("\n\nRegister 2 = %d", machine.cpu.r[2]);
    return 0;
}