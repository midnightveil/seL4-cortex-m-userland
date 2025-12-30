.syntax unified
.thumb

.section .text._start
.global _start
.type _start, %function

_start:

    bl main
@ if we return from main, loop infinitely
_loop:
    b _loop

.size _start, .-_start
