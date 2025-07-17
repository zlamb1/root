#include "boot.h"

.code16
.section .stage1, "ax"

.extern BOOT_STAGE2_START
.extern KERN_SECTORS

.global start
.global putchar
.global putstring

.global BOOT_DRIVE_NUMBER

start:
    cli
    xor %ax, %ax
    mov %ax, %ds
    mov %ax, %ss
    mov $BOOT_REAL_STACK, %sp
    ljmp $0, $boot

boot:
    mov %dl, BOOT_DRIVE_NUMBER
    sti
    mov $hello, %si
    call putstring
    call lba
    mov $goodbye, %si
    call putstring
    ljmp $0, $BOOT_STAGE2_START 

lba:
    movw $KERN_SECTORS, %ax
    movw $KERN_SECTORS, scratch
    movl kernel_lba, %eax
    movl %eax, disk_address_packet_lba
    movl kernel_lba+4, %eax
    movl %eax, disk_address_packet_lba+4
lba_read:
    cmpw $0, scratch
    je 1f
    movb $0x42, %ah
    movb BOOT_DRIVE_NUMBER, %dl
    mov $disk_address_packet, %si
    int $0x13
    jc fail
    addl $1, disk_address_packet_lba
    adcl $0, disk_address_packet_lba+4
    addw $0x20, disk_address_packet_seg
    decw scratch
    jmp lba_read
1:
    ret

fail:
    hlt
    jmp fail

putchar:
    movb $0xE, %ah
    movb $0, %bh
    int $0x10
    ret

putstring:
    lodsb
    cmp $0, %al
    je 1f
    call putchar
    jmp putstring
1:
    ret

BOOT_DRIVE_NUMBER:
.byte 0

scratch:
.word 0

disk_address_packet:
.byte 0x0010
.byte 0x0000
.word 0x0001
.word 0x7E00
disk_address_packet_seg:
.word 0x0000
disk_address_packet_lba:
.quad 0x0000

hello:
.asciz "enter boot.S\r\n"

goodbye:
.asciz "exit boot.S\r\n"

.org BOOT_KERNEL_LBA

kernel_lba:
.quad 1

.org 510
.word 0xAA55