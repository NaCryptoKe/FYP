#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdio.h>

#include "cpu.h"
#include "ISA.h"

Machine machine = {0};

int main(void) {
    start_machine(&machine);
    uint32_t pc = resolve_addr(machine.cpu.seg, machine.cpu.pc);
    uint32_t ld = resolve_addr(machine.cpu.seg, machine.cpu.load_addr);
    uint32_t sp = resolve_addr(0XFF, machine.cpu.sp);

    printf(
        "Program Counter: 0X%06" PRIX32 
        "\nCurrent Address: 0X%06" PRIX32 
        "\nStack Pointer: 0X%06" PRIX32 
        "\nZero Flag: %08b\n",
        pc, ld, sp, machine.cpu.flag
    );

    uint16_t inst = instruction_maker(MOVI, 0x03, 0x02, 0x04);  // (2 << 3) | 4 = 010100b = 20d = 0x14
    add_to_memory(inst, &machine);

	inst = instruction_maker(MOVI, 0x04, 0x02, 0x05);
    add_to_memory(inst, &machine);

    inst =  instruction_maker(MOV, 0x02, 0x03, 0x01);
    add_to_memory(inst, &machine);

    inst = instruction_maker(PUSH, 0x04, 0x04, 0x03);
    add_to_memory(inst, &machine);

    inst = instruction_maker(POP, 0x02, 0x02, 0x03);
    add_to_memory(inst, &machine);

    inst =  instruction_maker(ADD, 0x01, 0x02, 0x03);
    add_to_memory(inst, &machine);

    inst = instruction_maker(HALT, 0x00, 0x00, 0x00);
    add_to_memory(inst, &machine);

    cycle(&machine);
    return 0;
}
