/* src/linker.ld */
OUTPUT_FORMAT("binary")

SECTIONS
{
    /* The bootloader drops the kernel exactly at this memory address */
    . = 0x1000;

    /* Sector 1: Executable Instructions ONLY */
    .text : {
        *(.text)
    }

    /* Sector 2: Read-Only Variables (e.g., text strings) */
    .rodata : {
        *(.rodata)
    }

    /* Sector 3: Initialized Variables */
    .data : {
        *(.data)
    }

    /* Sector 4: Uninitialized / Zeroed Variables */
    .bss : {
        *(COMMON)
        *(.bss)
    }
}