#include "boot.h"

.section .stage2.util, "ax"

tmpaddr:
.long 0

protstack:
.long BOOT_PROT_STACK

realidt:
.quad 0
.word 0

protidt:
.quad 0
.word 0

.extern gdtdesc

.global enter_prot
.global enter_real
.global root_bios_interrupt
.global root_mach_reboot

.code16
enter_prot:
    cli
    sidt realidt
    lgdt gdtdesc
    mov %cr0, %eax
    or $0x1, %al
    mov %eax, %cr0
    ljmp $0x8, $pmode
.code32
pmode:
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %fs
    mov %ax, %es
    mov %ax, %gs
    mov %ax, %ss
    mov (%esp), %eax
    mov %eax, tmpaddr
    mov protstack, %eax
    mov %eax, %esp
    mov tmpaddr, %eax
    push %eax
    lidt protidt
    ret

enter_real:
    lgdt gdtdesc
    sidt protidt
    mov %esp, %eax
    add $4, %eax
    mov %eax, protstack
    mov (%esp), %eax
    mov $BOOT_REAL_STACK, %ebp
    mov %ebp, %esp
    push %eax
    ljmp $0x18, $pmode16
.code16
pmode16:
    mov $0x20, %ax
    mov %ax, %ds
    mov %ax, %fs
    mov %ax, %es
    mov %ax, %gs
    mov %ax, %ss
    lidt realidt
    mov %cr0, %eax
    and $0xFFFFFFFE, %eax
    mov %eax, %cr0
    ljmp $0x0, $rmode
rmode:
    xor %ax, %ax
    mov %ax, %ds
    mov %ax, %fs
    mov %ax, %es
    mov %ax, %gs
    mov %ax, %ss
    sti
    retl

# ;eax - int num
# ;edx - regs
.code32

root_bios_interrupt:
    pushf
    push %ebp
    push %eax
    push %ebx
    push %ecx
    push %esi
    push %edi
    push %edx
    movb %al, intnum
    movw 24(%edx), %ax
    movw %ax, register_flags
    movw 26(%edx), %ax
    movw %ax, register_ds
    movw 28(%edx), %ax
    movw %ax, register_es
    movl 0(%edx), %eax
    movl %eax, register_eax
    movl 4(%edx), %ebx
    movl 8(%edx), %ecx
    movl 12(%edx), %esi
    movl 16(%edx), %edi
    movl 20(%edx), %edx
    call enter_real
.code16
    cli
    mov %ds, %ax
    push %ax
    # movw imm16, %ax
    .byte 0xB8
register_es:
    .short 0
    mov %ax, %es
    # movw imm16, %ax
    .byte 0xB8
register_ds:
    .short 0
    mov %ax, %ds
    # movw imm16, %ax
    .byte 0xB8
register_flags:
    .short 0
    # movl imm32, %eax
    .byte 0x66, 0xB8 
register_eax:
    .long 0
    nop
    # int imm8
    .byte 0xCD
intnum:
    .byte 0
    mov %eax, %cs:register_eax
    pushf
    pop %ax
    mov %ax, %cs:register_flags
    mov %ds, %ax
    mov %ax, %cs:register_ds
    mov %es, %ax
    mov %ax, %cs:register_es
    pop %ax
    mov %ax, %ds
    calll enter_prot
.code32
    pop %eax
    mov %ebx, 4(%eax)
    mov %ecx, 8(%eax)
    mov %esi, 12(%eax)
    mov %edi, 16(%eax)
    mov %edx, 20(%eax)
    mov %eax, %edx
    mov register_eax, %eax
    mov %eax, 0(%edx)
    mov register_flags, %ax
    mov %ax, 24(%edx)
    mov register_ds, %ax
    mov %ax, 26(%edx)
    mov register_es, %ax
    mov %ax, 28(%edx)
    pop %edi
    pop %esi
    pop %ecx
    pop %ebx
    pop %eax
    pop %ebp
    popf
    ret
