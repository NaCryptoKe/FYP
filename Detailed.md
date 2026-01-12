# Detailed Explanation of the Laptop Design Document (LDD)

This document breaks down the design for a custom-built 16-bit computer processor (CPU) and the basics of its operating system (OS). It explains the technical choices made for this project in simple terms with added examples.

---

## 1. Core CPU Design

### What "16-bit" Means
The CPU is designed to be **16-bit**. This means it naturally handles information in chunks of 16 bits (which is equal to 2 bytes). 

*   **Data Path:** The "pathways" inside the CPU that data travels on are 16 bits wide.
*   **Performance:** Compared to an older 8-bit CPU running at the same speed, this design can theoretically process twice as much data in the same amount of time.

This size is a good middle-ground: it's more powerful than an 8-bit CPU but not as complex as a modern 32-bit or 64-bit CPU, making it achievable for a single person to design and build. All the main internal storage spots in the CPU, called registers, are 16 bits wide.

### How It Stores Numbers (Endianness)
The system uses a method called **Big Endian**. When a number is too big to fit in a single byte (like our 16-bit numbers, which use two bytes), this rule decides the order they are stored in memory. Big Endian means the most important, or "biggest," byte is stored first, at the lowest memory address.

*__Analogy:__ Think of writing down the number "257". In Big Endian, you'd store the "2" (most significant part) before the "57" (least significant part). This is often more intuitive for people to read when looking at raw memory data, as it matches how we normally read numbers.*

---

## 2. Memory System

### Accessing a Large Amount of Memory
While the CPU's internal parts are 16-bit (which would normally limit it to just 64 kilobytes (KB) of memory), the design uses a trick to access a much larger space: **16 megabytes (MB)**. This is achieved by having a 24-pin address bus, which can point to over 16 million different memory locations.

### The "Segmentation" Trick
To bridge the gap between its 16-bit brain and the 24-bit address bus, the CPU uses **Segmentation** (also called "Banking"). It views the entire 16 MB memory as being chopped up into **256 separate "Pages" or "Segments,"** each 64 KB in size.

To find any piece of data, the CPU generates an address by combining two things:
1.  An **8-bit Segment Register** that selects one of the 256 pages.
2.  A **16-bit Offset** that points to the exact location inside that page.

The hardware simply sticks these two numbers together to form the final 24-bit physical address. 
`[Segment (8 bits)] + [Offset (16 bits)] = Physical Address (24 bits)`

A side benefit of this is a basic form of security: a program running in one segment can't easily interfere with data in another segment without explicitly changing the Segment Register.

---

## 3. The CPU's Internal Registers

The CPU has a small, efficient set of internal storage slots called registers.

### General Purpose Registers (GPRs)
There are **8 main registers** (R0 through R7).
*   **R0–R6** are used for everyday calculations and storing memory addresses.
*   **R7** is a special "Extension Register." It's used when a calculation produces a result that is too big for a single 16-bit register. For example, if you multiply two large 16-bit numbers, the result could be up to 32 bits long. R7 is used to hold the extra, high-order bits of the result that "spill over."

---

## 4. The CPU's Language (Instruction Set)

### Fixed-Size Instructions
Every command, or "instruction," that the CPU can execute is exactly **16 bits long**. This fixed length makes the CPU's internal control logic much simpler, as it doesn't have to figure out how long each instruction is.

### How an Instruction is Structured
Each 16-bit instruction is divided into fields to maximize what can be done in a single command.

| Field | Bit Width | Description |
| --- | --- | --- |
| **Opcode** | **7 bits** | The command itself (e.g., `ADD`). Supports 128 unique instructions. |
| **Destination (Rd)** | **3 bits** | The register where the result will be stored. |
| **Source 1 (Rs1)** | **3 bits** | The first register providing data for the command. |
| **Source 2 (Rs2)** | **3 bits** | The second register providing data. |

### Example of an Instruction
Let's look at the assembly command `ADD R1, R2, R3`, which means "Add the value in R2 and R3, and store the result in R1."

The CPU sees this not as text, but as a 16-bit binary number broken down like this:
*   **Opcode (ADD):** `[some_7_bit_number]`
*   **Rd (R1):** `001`
*   **Rs1 (R2):** `010`
*   **Rs2 (R3):** `011`

The final instruction would be a single 16-bit value concatenating these parts.

---

## 5. Memory Map

The system's 16 MB of addressable space is carefully divided into regions with specific purposes.

| Address Range | Size | Purpose | Description |
|---|---|---|---|
| **$000000 – $00FFFF** | 64 KB | **ROM** | Read-Only Memory. This holds the permanent startup code (BIOS/Firmware). |
| **$010000 – $010FFF** | 4 KB | **MMIO** | Memory-Mapped I/O. Accessing hardware is done by writing to these special addresses. |
| **$011000 – $FEFFFF** | ~15.8 MB| **RAM** | Random-Access Memory. This is the main workspace for running programs. |
| **$FF0000 – $FFFFFF** | 64 KB | **High RAM**| This top portion of RAM is for the system stack and the Interrupt Vector Table. |

### Example: Accessing Hardware (MMIO)
Because an instruction can't hold a full 24-bit address, talking to hardware in the MMIO range is a two-step process. Let's say we want to write the character 'A' to a screen buffer located at physical address `$010500`.

1.  The target address is in the `$01`xxxx page. So first, we must load `$01` into the Segment register.
2.  Then, we use the offset `$0500` to pinpoint the exact location.

Here is what it looks like in assembly code:
```asm
; Step 1: Set the memory page (segment)
MOVI R2, $01       ; Load the page number 0x01 into a register
MOV SEG, R2        ; Move it to the special Segment Register

; Step 2: Access the specific location within that page
MOVI R3, $0500     ; Load the offset address
MOVI R1, 'A'       ; Load the data ('A')
STORE R1, [R3]     ; Store the data at the offset, which the hardware
                   ; combines to be physical address $010500
```

---

## 6. Bus Protocol (The Physical Connections)

This section defines the physical pins and electrical signals. It notes that key control signals use **Active-Low Logic**. This means a signal is triggered by a low voltage (ground) instead of a high voltage. This is a common and robust engineering practice for a few reasons:
1.  **Fail-Safe:** If a wire breaks, a pull-up resistor can ensure the signal defaults to "off," preventing accidental triggers.
2.  **Efficiency:** Standard electronic components are often faster at pulling a signal low than pulling it high.
3.  **Wired-OR:** It allows multiple devices to be connected to the same line. Any one of them can pull the line low to send a signal without interfering with the others.

---

## 7. The Segmentation Unit (How It's Built)

This explains the simple but clever hardware implementation of the segmentation system. There are no complex circuits for calculating addresses. Instead, it's done by direct wiring.

*__Analogy:__ Think of a phone number. The 8-bit Segment is like the **area code**, and the 16-bit Offset is the **local number**. The hardware simply dials them one after the other to connect to the final destination.*

*   The top 8 wires of the address bus (A16-A23) are **directly connected** to the 8-bit Segment Register.
*   The bottom 16 wires (A0-A15) are **directly connected** to the CPU's internal 16-bit address bus.

This physically concatenates the two values to create the 24-bit address, making it extremely fast and simple.

---

## 8. Handling Interrupts

Interrupts are signals from hardware that need the CPU's immediate attention.

### Interrupt Vector Table (IVT)
The IVT is a list of addresses located in RAM, starting at address `$FF0000`. For each type of interrupt, there is an entry in this table that points to the code (the "Interrupt Service Routine" or ISR) that should be run. It's in RAM so that the OS can change these handlers dynamically.

| Interrupt ID | Physical Address | Default Handler For |
| --- | --- | --- |
| **00** | `$FF0000` | Divide by Zero Exception |
| **01** | `$FF0002` | Non-Maskable Interrupt (e.g., power failure) |
| **02** | `$FF0004` | Timer Tick (used for multitasking) |
| **03** | `$FF0006` | Keyboard / UART Input |

### The Interrupt Process
When a device requests an interrupt:
1.  **Context Save:** The CPU automatically saves its current work. It pushes its status (FLAGS), current memory Segment (SEG), and position in the program (PC) onto the stack.
2.  **Vector Fetch:** It forcibly switches its internal Segment Register to `$FF` to access the High RAM area where the IVT is stored. It then reads the address of the correct handler from the table.
3.  **Execute:** The CPU jumps to that handler's address and executes the code.
4.  **Return:** Once finished, a special `RETI` (Return from Interrupt) instruction restores the original context, and the CPU continues exactly where it left off.

---

## 9. System Protection (Privilege Levels)

To prevent user programs from crashing the whole system, the CPU has two modes of operation, tracked by a bit in the FLAGS register.

*   **User Mode (P=0):** A restricted mode for running applications. Critical instructions are forbidden.
*   **System Mode (P=1):** An unrestricted mode for the OS kernel. The CPU has full control.

The CPU runs in User Mode by default and **automatically switches to System Mode** whenever an interrupt occurs, ensuring the OS can handle it with full privileges.

### The Status Register (FLAGS)
This is a special 16-bit register where each bit is a "flag" that represents a different status of the CPU.

| Bit | Mnemonic | Name | Function |
| --- | --- | --- | --- |
| **15** | **P** | **Privilege** | `1` = System Mode, `0` = User Mode. |
| **14** | **I** | **Int Enable** | `1` = CPU will respond to hardware interrupts. |
| **13** | **S** | **Step** | `1` = Puts CPU in a debug mode. |
| **0-3** | **Z/C/N/V** | **ALU Flags** | Standard flags: Zero, Carry, Negative, Overflow. |

---

## 10. Physical Pins
This section lists some of the dedicated physical pins on the CPU chip that are used for handling different kinds of interrupts, such as the general **Interrupt Request (IRQ)** and the critical **Non-Maskable Interrupt (NMI)** which is used for major system events like a power failure warning.
