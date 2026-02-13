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

### Status
- [X] Bootloader (Real Mode -> Protected Mode)
- [X] GDT / IDT Setup
- [X] Paging & Page Tables
- [ ] Jump to Long Mode
- [ ] Kernel Main Entry
- [ ] Memory Management
- [ ] Shell

---
*Created by S3cr3tpt. Started Feb 2026.*