.syntax unified
.thumb

.extern _stack_top
.extern _sbss, _ebss
.extern _sdata, _edata, _sdata_load

.section .text._start
.global _start
.type _start, %function
_start:
    /* relocate & zero */

    ldr r0, =_sbss  @ incremented during loop
    ldr r1, =_ebss
    @ fill value
    movs r2, #0
zero_bss_begin:
    cmp r0, r1
    @ exit out if r0 >= r1
    bhs zero_bss_done
    @ zero out r0; word-sized stores; then r0 += 4
    str r2, [r0], #4
    b zero_bss_begin
zero_bss_done:

    ldr r0, =_sdata       @ r0: addr, incremented
    ldr r1, =_edata       @ r1: end addr
    ldr r2, =_sdata_load  @ r2: loadaddr, incremented
relocate_compare:
    cmp r0, r1
    @ exit if r0 (addr) >= r1 (end)
    bhs relocate_done

    @ copy from load to addr and increment
    ldr r3, [r0], #4
    str r3, [r2], #4

    b relocate_compare

relocate_done:
    /* start C */

    ldr sp, =_stack_top
    @ capRegister (r4) contains the bootinfo frame
    mov r0, r4
    bl start
@ if we return from main, loop infinitely
loop:
    b loop

.size _start, .-_start
