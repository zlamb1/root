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
    mov %ax, %es
    mov %ax, %ss
    mov $STAGE1_STK, %ax
    mov %ax, %sp
    ljmp $0, $boot

boot:
    sti
    mov $ENTER_STRING, %si
    call prints

    # check int 0x13/ah=42h support
    mov $0x41, %ah
    mov $0x55AA, %bx
    mov DRIVE_NUMBER, %dl
    int $0x13
    jc 1f
    cmp $0xAA55, %bx
    jne 1f
    and $1, %cx
    jz 1f
    call lba
    jmp 2f
1:
    call chs
2:
    mov $EXIT_STRING, %si
    call prints
    ljmp $0, $STAGE2_LOC

chs:
    mov $0x8, %ah
    mov DRIVE_NUMBER, %dl
    mov $0, %di
    int $0x13
    jc fail
    and $0x3F, %cl
    mov %cl, SECTORS_PER_TRACK
    mov %dh, NUMBER_OF_HEADS
    cmp $0, %cl
    je fail
    cmp $0, %dh
    je fail
chs_read:
    mov READ_SECTORS, %ax
    cmp $0, %ax
    je 1f
    mov DISK_ADDRESS_PACKET_BLOCK_NUMBER_L0, %ax
    divb SECTORS_PER_TRACK
    mov %ah, %cl
    inc %cl
    xor %ah, %ah
    divb NUMBER_OF_HEADS
    mov %ah, %dh
    mov %al, %ch
    mov $0x2, %ah
    mov $1, %al
    mov DRIVE_NUMBER, %dl
    mov $STAGE2_LOC, %bx
    int $0x13
    jc chs_retry
    incw DISK_ADDRESS_PACKET_BLOCK_NUMBER_L0
    decw READ_SECTORS
    mov %es, %ax
    add $0x20, %ax
    mov %ax, %es
    jmp chs_read
1:
    xor %ax, %ax
    mov %ax, %es
    ret

chs_retry:
    incb READ_ATTEMPTS
    mov READ_ATTEMPTS, %al
    cmp $3, %al
    jg fail
    mov $0, %ah
    mov DRIVE_NUMBER, %dl
    int $0x13
    jc fail
    jmp chs_read

lba:
    mov READ_SECTORS, %eax
    cmp $0, %eax
    je 1f
    mov $0x42, %ah
    mov $DISK_ADDRESS_PACKET, %si
    mov DRIVE_NUMBER, %dl
    int $0x13
    jc fail
    mov DISK_ADDRESS_PACKET_SEGMENT, %ax
    add $0x20, %ax
    mov %ax, DISK_ADDRESS_PACKET_SEGMENT
    addl $1, DISK_ADDRESS_PACKET_BLOCK_NUMBER_L0
    adcl $0, DISK_ADDRESS_PACKET_BLOCK_NUMBER_L1
    decl READ_SECTORS
    jmp lba
    1:
    ret

puts:
    mov $0xE, %ah
    mov $0, %bh
    int $0x10
    ret

prints:
    1:
    cld
    lodsb
    cmp $0, %al
    je 2f
    call puts
    jmp 1b
    2:
    ret

fail:
push %ax
mov $FAIL_STRING, %si
call prints
pop %ax
mov %ah, %al
xor %ah, %ah
call itoa
jmp stop

itoa:
mov $10, %bl
div %bl
push %ax
xor %ah, %ah
cmp $0, %al
je 1f
call itoa
1:
pop %ax
mov %ah, %al
add $'0', %al
call puts
ret

stop:
hlt
jmp stop

SECTORS_PER_TRACK:
.byte 0

NUMBER_OF_HEADS:
.byte 0

READ_ATTEMPTS:
.byte 0

READ_SECTORS:
.long STAGE2_SEC

.align 4
DISK_ADDRESS_PACKET:
    .byte  0x0010
    .byte  0x0000
    .short 0x0001
    .short STAGE2_LOC
DISK_ADDRESS_PACKET_SEGMENT:
    .short 0x0000
DISK_ADDRESS_PACKET_BLOCK_NUMBER_L0:
    .long 0x0001
DISK_ADDRESS_PACKET_BLOCK_NUMBER_L1:
    .long 0x0000

DISK_CYLINDER:
    .byte 0

.global DRIVE_NUMBER

DRIVE_NUMBER:
.byte 0x0

ENTER_STRING:
.asciz "enter stage1\r\n"

FAIL_STRING:
.asciz "disk error\r\n"

EXIT_STRING:
.asciz "exit stage1\r\n"

. = 510
.byte 0x55, 0xAA