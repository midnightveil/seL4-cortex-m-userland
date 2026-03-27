INCLUDE memory.ld

SECTIONS
{
    .text : ALIGN(4) {
        KEEP(*(.text._start))
        *(.text)
        *(.text.*)
    } >FLASH

    .rodata : ALIGN(4) {
        *(.rodata) *(.rodata.*)
    } >FLASH

    .data : ALIGN(4) {
        _sdata = .;
        *(.data) *(.data.*)
        _edata = .;
    } >SRAM AT>FLASH

    _sdata_load = LOADADDR(.data);

    .bss (NOLOAD) : ALIGN(4) {
        _sbss = .;

        _stack_bottom = ALIGN(4);
        . += 0x1000;
        _stack_top = .;

        *(.bss)
        *(.bss.*)

        _ebss = .;
    } >SRAM

    /DISCARD/ : {
        *(.ARM.exidx)
    }
}
