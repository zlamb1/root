.code16

.section .stage2.text, "ax"

.extern has_a20
.extern set_a20

start2:
    call set_a20
    test %ax, %ax
    jnz 1f
    stop:
    hlt
    jmp stop
    1:
    cli
    calll enter_prot
.code32
    and $-16, %esp
    jmp root_main

.extern root_main