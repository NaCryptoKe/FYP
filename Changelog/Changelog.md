# I. Planning

## 1. Problem statement

Create an emulator capable of executing a tiny subset of the CPU instruction that I wrote.

## 2. Invariants

The following only upholds for V.0.1

- PC always points to the next instruction
- R0-R7 are always 16-bit values
- One instruction at a time per CPU cycle
- HALT set the boolen of uint8_t flag to stop the cycle
- Opcode must be below 7 bit

# II. Design and build

## 1. Designing the data

In basic terms I'm designing the bare minimum of the CPU. The CPU needs the 8 register (R0-R7) **(Invariant 2)**, a PC to point to the next instruction **(Invariant 1)**, a halt flag to let the CPU know its halt state (**Invarint 4)**

``` C
typedef struct {
    uint16_t r[8];      // This is R0-R7
    
    uint16_t pc;        // program counter register

    uint8_t seg;        // the 8-bit segment register used when offsetting the 16-bit location

    uint8_t halt;       // this is basically a bool flag for halt, 1 means halt 0 means not
} CPU;
```

## 2. Scope a brutal MVP

What I want basically in this version is for it to have the following capabilities:

- Crete a machine with basic memory no segmentation

``` C
typedef struct{
    CPU cpu;
    uint8_t memory [65536]; //64 KiB of RAM
} Machine;
```

- Load to register with opcode 0x01     *INST_SET*
- Load to halt flag with opcode 0x03    *INST_HALT*


## 3. Build one vertical slice at a time. input -> process -> output

### First vertical slice is the fetch

The fetch cycle basically get data that will be processed

``` C
uint16_t fetch(Machine *machine) {
    uint16_t high = machine->memory[machine->cpu->pc];
    uint16_t low = machine->memory[machine->cpu->pc +1];

    machine->cpu->pc += 2;

    return (high << 8) | low;   // basically add the high and low bit to get the actual data, using Big Endian logic
}
```

# Current

Currently it can load to memory, make instruction, address putting data to a value and making it halt

# New changes

Currently what I'm doing is trying to implement the ROM into the segmentation. Basically implementing the segmentation of the ram to see how to do it, and because I need the stack.

Also thinking of adding a time management tool, that is used for basically adding a clock for the fetch, decode and execute cycle that I can control from being 3 GHZ to 1 KHz if I want to.