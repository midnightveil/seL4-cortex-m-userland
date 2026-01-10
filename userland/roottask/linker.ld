MEMORY
{
    SRAM (rwx)  : ORIGIN = 0x20040000, LENGTH = 520K
}

SECTIONS
{
    .initial_stack : {
        . += 4096;
    }

    .text : {
        KEEP(*(.text._start))
        *(.text)
    }

    /DISCARD/ : {
        *(.ARM.exidx)
    }
}
