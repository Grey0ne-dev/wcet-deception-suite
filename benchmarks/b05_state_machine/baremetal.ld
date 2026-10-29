ENTRY(main)

SECTIONS
{
    . = 0x00008000;

    .text : ALIGN(4)
    {
        KEEP(*(.text.main))
        *(.text*)
    }

    .rodata : ALIGN(4) { *(.rodata*) }
    .data   : ALIGN(4) { *(.data*) }
    .bss    : ALIGN(4)
    {
        __bss_start__ = .;
        *(.bss*)
        *(COMMON)
        __bss_end__ = .;
    }

    /DISCARD/ :
    {
        *(.comment*)
        *(.note*)
        *(.eh_frame*)
    }
}
