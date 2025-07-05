.section .stage2.data, "aw"

.extern exception_handler
.extern root_pic_eoi
.extern root_clock
.extern root_recv_sc

.global root_isr_stub_table
.global root_irq_stub_table
.global root_pit_irq
.global root_kb_irq

.macro isr_err_stub n
isr_stub_\n:
    sub $4, %esp
    and $-16, %esp
    movl $\n, 0(%esp)
    cld
    call exception_handler
    iret
.endm

.macro irq_stub n
irq_stub_\n:
    push %ebp
    mov %esp, %ebp
    sub $16, %esp
    and $-16, %esp
    mov %eax, 12(%esp)
    mov %ecx, 8(%esp)
    mov %edx, 4(%esp)
    movl $\n, 0(%esp)
    cld
    call root_pic_eoi
    mov 12(%esp), %eax
    mov 8(%esp), %ecx
    mov 4(%esp), %edx
    leave
    iret
.endm

.rept 32
isr_err_stub \+
.endr

.rept 16
irq_stub \+
.endr

root_isr_stub_table:
.rept 32
.long isr_stub_\+
.endr

root_irq_stub_table:
.rept 16
.long irq_stub_\+
.endr

root_pit_irq:
    push %ebp
    mov %esp, %ebp
    sub $12, %esp
    and $-16, %esp
    mov %eax, 8(%esp)
    mov %ecx, 4(%esp)
    mov %edx, 0(%esp)
    cld
    call root_clock
    mov 8(%esp), %eax
    mov 4(%esp), %ecx
    mov 0(%esp), %edx
    leave
    iret

root_kb_irq:
    push %ebp
    mov %esp, %ebp
    sub $12, %esp
    and $-16, %esp
    mov %eax, 8(%esp)
    mov %ecx, 4(%esp)
    mov %edx, 0(%esp)
    cld
    call root_recv_sc
    mov 8(%esp), %eax
    mov 4(%esp), %ecx
    mov 0(%esp), %edx
    leave
    iret