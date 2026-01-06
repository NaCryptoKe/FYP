# Laptop Design Document

## CPU

### 1. CPU bitwidth

The bit width of a CPU affects the amount of data that a processor can process in a single operation.

So for my FYP project I decided on **16** bit width, which high enough so that we can use simple GUI but not too high which will be a daunting task for a solo developer.

1 byte = 8 bits, so because mine is **16 bits** that equates to **2 bytes** of data at a time.
This means in theory that the CPU I'm making will be twice as fast as a 8 bits CPU running on the same frequency.

> So the bitwidth is locked on **16 BITS**

### 2. Address space size

The address space size is the amount of memory a cpu can access at a given time.

Normally it is calculated by $n^2$, which in theory will make my cpu hold around $2^{16}$ which is around **64 KB** of memory address space size. 

Inspired by the **INTEL 80286** CPU, I will try to implement a data bus of 16 pins (D0 - D15), and address bus of 24 pins (A0-A23) making a ram size of equivalent to **16 MB**.

| Component    | Specification | Impact on the system                         |
|--------------|---------------|----------------------------------------------|
| Internal CPU | 16-bit        | Can process numbers up to **65,535** at once |
| Data Bus     | 16-bit        | Moves 2 bytes (*One Word*) at a time         |
| Address Bus  | 24-bit        | Can physically connect to **16 MBs** of RAM  |

Because a 16-bit cpu can actually use a max of *64 KB* of memory I will be using a trick called *`Segmentation`*. 

1. It uses two 16-bit registers to calculate a single address
2. One of the registers (**Segment Register**) points to the general neighbourhood of *64KB*
3. The second register (**The Offset Register**) points to the exact memory inside the neighbourhood
4. An internal **Address Unit (AU)** does the maths to combine these two into a single 24-bit signal sent to the physical address bus pins

This is the first era where true multitasking occured, leading a path to **Protected Mode**. Where one program couldn't "see" or crash the memory of another.

The CPU still thinks there is only **64KB** of max memory to point at, but the segmentation trick bypasses that.

I didn't make the internal CPU 24-bit because it will have required more transistors than a 16-bit CPU without giving me much of performance.

### 3. Endianness

Endianness refers to the order in which bytes of a word are transmitted over a data communication.

There are two variation, **Big Endian** and **Little Endian**

I chose **Big Endian** because it's human readable, and will be easier to debug when eventually I encounter a bug.

If a memory started at point a, in big endian it goes like a, a+1 for a 16 bit CPU.

### 4. Instruction encoding format

This defines how instructions are represented in a computer's memory. 

| Field                   | Purpose                                                                                 |
|-------------------------|-----------------------------------------------------------------------------------------|
| Opcode (Operation Code) | Tells the CPU what to do (e.g., ADD, JUMP, LOAD).                                       |
| Operands                | Tells the CPU what to act on (e.g., Register numbers, Memory addresses).                |
| Addressing Mode         | Tells the CPU how to find the data (e.g., Is it a constant? Is it at a memory address?).|

I will be using **Variable-Length** (The CISC approach)

Instructions can be different sizes (e.g., 1 byte, 3 bytes, or 15 bytes).

Example: Intel x86 or the 8-bit 6502.

How it works: The CPU reads the first byte (the Opcode). That byte contains a "hidden" signal that tells the CPU: "I am a 3-byte instruction; please go fetch two more bytes before you try to run me."

Pro: Very memory-efficient. Small commands take small space.

Con: The CPU has to work harder to "decode" the length, which can slow down the pipeline.

I wanted it to be more memory-efficient rather power-efficient for now, but will try to achieve both.

I will be using a highbrid of Three and Two Instruction encoding, the readability of Three Instruction encoding with two address system's compactness.

How I will do it is, using AX(Accumulator Register) and EX (Extended Register) as the designated arithmetic storages.

For Example:
``` text
add bx, cx
```

The "Balanced" Design **(8 Registers)**

Limiting the cpu to 8 registers (R0 through R7), will only need 3 bits per register ($2^3=8$).

Rd​: 3 bits, Rs1​: 3 bits, Rs2​: 3 bits

Total for Registers: 9 bits.

Remaining for Opcode: $16 − 9 = 7$ bits.

**The Result:** Now the CPU can have 128 different instructions. In return making it much more practical for a real-world CPU implementation.

R7 can't be used, it will be used for **Widening Operation**, where the spill off will be stored