.code32
.section .stage2.util

.global root_isr_stub_table
.global root_irq_stub_table
.global root_irq_table

.extern exception_handler

.macro isr_err_stub n
isr_stub_\n:
    sub $4, %esp
    and $-16, %esp
    movl $\n, 0(%esp)
    cld
    call exception_handler
    iret
.endm

.rept 32
isr_err_stub \+
.endr

root_isr_stub_table:
.rept 32
.long isr_stub_\+
.endr

.macro m_irq_stub n
irq_stub_\n:
    push %ebp
    mov %esp, %ebp
    sub $12, %esp
    and $-16, %esp
    mov %eax, 8(%esp)
    mov %ecx, 4(%esp)
    mov %edx, 0(%esp)
    mov $\n, %eax
    mov $4, %ecx
    mul %ecx
    mov $root_irq_table, %ecx
    mov (%ecx,%eax), %eax
    cmp $0, %eax
    je 1f
    cld
    call *%eax
1:
    mov 8(%esp), %eax
    mov 4(%esp), %ecx
    mov 0(%esp), %edx
    leave
    iret
.endm

.rept 16
m_irq_stub \+
.endr

root_irq_stub_table:
.rept 16
.long irq_stub_\+
.endr

root_irq_table:
.rept 16
.long 0
.endr