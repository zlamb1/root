.code16

.extern STAGE1_STK
.extern STAGE2_LOC
.extern STAGE2_SEC

.section .stage1.text, "ax"

.global start
.global prints

start:
    cli
    mov %dl, DRIVE_NUMBER
    xor %ax, %ax
    mov %ax, %ds
    mov %ax, %ss
    mov $STAGE1_STK, %ax
    mov %ax, %sp
    ljmp $0, $boot

boot:
    sti
    mov $ENTER_STRING, %si
    call prints
    mov $0x41, %ah
    mov $0x55AA, %bx
    mov DRIVE_NUMBER, %dl
    int $0x13
    mov $0x42, %ah
    mov $DISK_ADDRESS_PACKET, %si
    mov DRIVE_NUMBER, %dl
    int $0x13
    mov $EXIT_STRING, %si
    call prints
    ljmp $0, $STAGE2_LOC

prints:
    1:
    cld
    lodsb
    cmp $0, %al
    je 2f
    mov $0xE, %ah
    mov $0, %bh
    int $0x10
    jmp 1b
    2:
    ret

.align 4
DISK_ADDRESS_PACKET:
    .byte  0x0010
    .byte  0x0000
    .short STAGE2_SEC
    .short STAGE2_LOC
    .short 0x0000
    .long  0x0001
    .long  0x0000

.global DRIVE_NUMBER

DRIVE_NUMBER:
.byte 0x0

ENTER_STRING:
.asciz "enter stage1\r\n"

EXIT_STRING:
.asciz "exit stage1\r\n"

. = 446

.macro part n
partition\n:
    p\n\()_drive: 
        .byte 0
    p\n\()_first_sector: 
        .byte 0, 0, 0
    p\n\()_type: 
        .byte 0
    p\n\()_last_sector: 
        .byte 0, 0, 0
    p\n\()_first_sector_lba: 
        .byte 0, 0, 0, 0
    p\n\()_num_sectors: 
        .byte 0, 0, 0, 0
.endm

part 1
part 2
part 3
part 4

. = 510
.byte 0x55, 0xAA