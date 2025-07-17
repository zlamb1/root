#include "boot.h"

.code16
.section .stage2, "ax"

.extern putstring
.extern set_a20
.extern enter_prot

.extern KERN_BSS_START
.extern KERN_BSS_END

.extern root_memset
.extern root_main

start2:
    mov $hello, %si
    call putstring
    call set_a20
    test %ax, %ax
    jnz 1f
    mov $a20_fail, %si
    call putstring
stop:
    hlt
    jmp stop
1:
    mov $goodbye, %si
    call putstring
    jmp 1f

hello:
.asciz "enter entry.S\r\n"

a20_fail:
.asciz "fail A20\r\n"

goodbye:
.asciz "leave entry.S\r\n"

1:
    calll enter_prot

.code32
enter_main:
    and $-16, %esp
    cld
    call zero_bss
    jmp root_main

loop:
    hlt
    jmp loop

zero_bss:
    push %ebp
    mov %esp, %ebp
    sub $12, %esp
    and $-16, %esp
    mov $KERN_BSS_END, %eax
    sub $KERN_BSS_START, %eax
    mov %eax, 8(%esp)
    movl $0, 4(%esp)
    movl $KERN_BSS_START, 0(%esp)
    call root_memset
    leave
    ret