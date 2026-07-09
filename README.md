# Ex-NihilOS
> *Ad Astra Per Aspera* (To the stars through difficulties)

### 0x00: The Origin
**Ex-NihilOS** is a 64-bit Operating System kernel built from absolute zero. 

There are no libraries here. No borrowed code. No "black boxes."
This project is an exercise in absolute control. It is an attempt to reconstruct the machine's reality byte by byte, moving from the nothingness of a blank sector to a fully operational, keyboard-driven environment.

### 0x01: Philosophy
The universe is chaotic; the CPU is deterministic.
This OS is my sanctuary. It is built on the belief that if you control the memory, you control the system.

### 0x02: Architecture
* **Arch:** x86_64
* **Boot:** Custom Assembly Bootloader (Stage 1 & 2)
* **Kernel:** C / Assembly Mix
* **User Space:** Tiling TUI (Text User Interface) - No Mouse.
* **Objective:** Malware Analysis Sandbox & Bare Metal Education.

### 0x03: The DevLog
This OS carries its own history.
The development process—the struggles, the triple faults, and the logic—is documented in the [DEVLOG.md](./DEVLOG.md).
*(Note: The DevLog is eventually embedded into the OS filesystem itself).*

### 0x04: Deployment
To compile the architecture and launch the QEMU emulator:
`chmod +x build.sh && ./build.sh`

### 0x05: Technical Specs
- **Mode:** 64-bit Long Mode (DPL 0)
- **Video:** VESA VBE 1920x1080x32bpp
- **Interrupts:** 256-gate IDT (ISR/IRQ Handling)
- **Memory:** 4-level Paging (Identity Mapped at 0x70000)
- **Storage:** ATA PIO Mode (2MB Raw Platter)

### 0x06: Phase 1 Status (Foundation & REPL)
- [X] Bootloader (Stage 1: Disk Read & Jump)
- [X] Kernel Entry (Stage 2: 16-bit Setup -> 64-bit Long Mode)
- [X] GDT / IDT Setup
- [X] Paging (4-Level Identity Map + Huge Pages)
- [X] VESA High-Definition Graphics (1920x1080)
- [X] 64 bits IDT 
- [X] Memory Management (Physical/Virtual Matrix)
- [X] Heap Allocator (Dynamic Memory Slicing)
- [X] PIC Remapping & IRQ Unmasking
- [X] PS/2 Keyboard Driver 
- [X] ASCII Software Renderer (Text I/O)
- [X] Tiling TUI / Shell

### 0x07: Phase 2 Trajectory (Persistent Data & Kinetics)
To elevate this architecture from a bare-metal execution loop to a resilient system, the following vectors map the current operational trajectory.

 - [x] Single Address Space (Ring 0): Bypassed Ring 3 privilege segregation. The architecture operates entirely in Ring 0 for zero-latency, absolute-authority execution.
 - [x] Storage Hardware Bridge: Engineered an ATA PIO disk driver capable of raw 512-byte magnetic sector reads/writes.
 - [x] Filesystem Foundation: Deployed a UNIX-style routing table (FileNodes) mapping directory logic to Logical Block Addresses (LBAs). Includes autonomous zero-state formatting and directory traversal (ls, cd).
 - [x] Kinetic TUI Matrix: Partitioned graphical text interface with real-time UI tracking (Virtual Keyboard matrix, Filesystem telemetry panel).
 - [ ] Data Payload Operations: Expand the filesystem architecture to support dynamic file creation (touch, mkdir) and raw data extraction (cat).
 - [ ] Native Buffer Editor: Engineer a bare-metal text editor allowing the system to natively write and save the DEVLOG.md directly to the SSD from within the active OS.
 - [ ] Hardware Interrupt Timers (PIT): Wire the Programmable Interval Timer (PIT) to execute asynchronous background hardware interrupts, driving autonomous telemetry updates (uptime, memory load) without blocking the REPL shell.
 - [ ] Binary Execution Pipeline: Construct an ELF dynamic loader to pull compiled binaries from the SSD into RAM and hand off the instruction pointer, allowing the OS to execute external files.
---
*Created by S3cr3tpt. Started Feb 2026.*