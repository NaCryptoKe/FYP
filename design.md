# Core Architecture Philosophy

**Project:** Creating a custom 16-bit CPU architecture with segmentation to 24 bits.

---

## 1. Core Architecture Philosophy & Bit-Width

Every instruction set architecture (ISA) is shaped by its economic and physical constraints. In a massive enterprise, teams of engineers can spend years to make a 32-bit or even more. For a solo developer, however, it is pragmatic and the main culprit is not logic gates, it is time.

The design of this architecture delicately balances software capability with hardware feasibility.

### 1.1 The Bit-Width Sweet Spot

When choosing the foundational width of the data path, my architecture had to steer between two extremes:

- **The 8-bit limitation**: While trivial to implement in hardware, you can even follow a tutorial from ben-eater, an 8-bit architecture imposes agonizing constraints on any kind of modern software architecture. Managing a simple 16-bit pointer or executing basic 16-bit arithmetic turns into a multi-cycle nightmare for developers to debug from register thrashing and complex bank-switching software routines. It is to small to host a multitasking operating system environment, which I hope to create on top of it.

- **The 32-bit abyss:** Conversely, going to a 32-bit development explodes complexity exponentially. A 32-bit arithmetic control unit (ALU) requires massive barrel shifters, complex forwarding logic to handle pipeline hazards, and a sprawling physical footprint that can easily balloon from months of solo development to years if not decades.

By locking the internal data paths and General Purpose Registers **(GPRs)** to **16-bits**, the system achieves harmonious sweet spot. It provides roughly double the operand width per instruction data throughput per clock-cycle compared to an identical 8-bit core, offering rich environment for software while remaining completely manageable for a solo hardware architect.

### 1.2 The Endianness Paradigm & Network Alignment

Byte ordering is the silent protocol dictating how processors view memory. My architecture deliberately adopts a Big Endian variation.

```
Memory Address:     $0000       $0001
                   +-----------+-----------+
Big Endian:        |    MSB    |    LSB    |    -> Reads left-to-right (Human Intuitive)
                   +-----------+-----------+
```

For me, this is a highly needed quality-of-life design choice for debugging later. When staring at raw memory hex dump during low-level debugging, a 16-bit value like `$1234` appears as `12 34`, rather than how little endian does it `34 12`.

Reading memory left to right is second nature for human cognition, which drastically reduces the mental fatigue during late-night debugging sessions.

Furthermore, while desktop OS and compilers like the likes of LLVM heavily favor *Little Endian*, the global internet routing infrastructure relies entirely on *Big Endian*. Because of this fact TCP/IP's "Network Byte Order" mandates Big Endian, making this CPU core inherently possess a native advantage in serialization, processing incoming network data streams without the latency overhead of byte-reversal.

To bridge the gap with the *Little Endian* world, the ISA introduces a dedicated **Byte-Swap** `(BSWAP)` instruction. This primitive can flip a 16-bit register's byte order in a single clock cycle, ensuring seamless interoperability with foreign data structures while preserving the pristine clarity of a Big-Endian memory space.


