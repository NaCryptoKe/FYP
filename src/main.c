#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include "cpu.h"
#include "ISA.h"

Machine machine = {0};

int main(void) {
    uint16_t inst = instruction_maker(MOVI, 0x03, 0x02, 0x04);
    add_to_memory(inst, &machine);

    inst =  instruction_maker(MOV, 0x02, 0x03, 0x01);
    add_to_memory(inst, &machine);

    inst =  instruction_maker(ADD, 0x01, 0x02, 0x03);
    add_to_memory(inst, &machine);

    inst = instruction_maker(HALT, 0x00, 0x00, 0x00);
    add_to_memory(inst, &machine);

    cycle(&machine);
    return 0;
}