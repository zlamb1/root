.code16

.section .stage2.text, "ax"

.extern prints

.extern has_a20
.extern set_a20

start2:
    mov $ENTER_STRING, %si
    call prints
    call set_a20
    test %ax, %ax
    jnz 1f
    stop:
    hlt
    jmp stop
    1:
    mov $EXIT_STRING, %si
    call prints
    calll enter_prot
.code32
    jmp enter_main
.code16
ENTER_STRING:
    .asciz "enter stage2\r\n"

EXIT_STRING:
    .asciz "exit stage2\r\n"

.code32
enter_main:
    and $-16, %esp
    cld
    call zero_bss
    jmp root_main

zero_bss:
    push %ebp
    mov %esp, %ebp
    sub $12, %esp
    and $-16, %esp
    mov $_BSS_END, %eax
    sub $_BSS_START, %eax
    mov %eax, 8(%esp)
    movl $0, 4(%esp)
    movl $_BSS_START, 0(%esp)
    call root_memset
    leave
    ret

.extern _BSS_START
.extern _BSS_END

.extern root_main
.extern root_memset
