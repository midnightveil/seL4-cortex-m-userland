.syntax unified
.thumb

.section .text._start
.global _start
.type _start, %function

_start:
    @ capRegister (r4) contains the bootinfo frame
    mov r0, r4
    bl start
@ if we return from main, loop infinitely
_loop:
    b _loop

.size _start, .-_start
