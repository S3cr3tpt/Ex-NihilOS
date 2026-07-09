/* src/linker.ld */
OUTPUT_FORMAT("binary")

SECTIONS
{
    /* [PATCH]: Shift the OS to the 64KB mark (0x10000) */
    . = 0x10000;

    .text : {
        *(.text)
    }

    .rodata : {
        *(.rodata)
    }

    .data : {
        *(.data)
    }

    .bss : {
        *(COMMON)
        *(.bss)
    }
}