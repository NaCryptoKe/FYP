#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <time.h>

#define MEMORY_SIZE 16 * 1024 * 1024        // 16 MiB
#define STACK_SIZE  64 * 1024               // 64 KiB for stack
#define CLOCK_SPEED 1000000                      // Clock speed of the virtual cpu, currently at 10Hz

typedef struct {
    uint16_t r[8];      // Invariant 2- stating the 16 bit R0-R7
    
    uint16_t pc;        // Invariant 1 - A register pointing to the next instruction
    uint16_t load_addr; // pointer to the current free ram slot

    uint8_t seg;        // the 8-bit segment register used when offsetting the 16-bit location
    uint16_t sp;         // stack pointer, to the top of the stack
    
    uint8_t flag;
    /*
    * Zero Flag
    * Carry Flag
    * Sign Flag
    * Overflow Flag
    * Parity Flag
    * Direction Flag    string processing direction experimental
    * Interrupt Flag    enables or disables hardware interrupts
    * 0 IF DF PF OF SF CF ZF, 1 is true and 0 is false
    * 0 0 0 0 0 0 0 0
    */

    uint8_t halt;       // Invariant 4 - this is basically a bool flag for halt, 1 means halt 0 means not
} CPU;

typedef struct {
    CPU cpu;
    uint8_t memory[MEMORY_SIZE];        // 8 bit because memory fundamentally is a single byte
} Machine;

uint32_t resolve_addr(uint8_t seg, uint16_t offset);
uint16_t fetch(Machine *machine);
void cycle(Machine *machine);
uint16_t instruction_maker(uint8_t mode, uint8_t opcode, uint8_t r1, uint8_t r2);
void add_to_memory(uint16_t inst, Machine *machine);

void start_machine(Machine *machine);
void throttle(double hz);

#endif  //CPU_H
