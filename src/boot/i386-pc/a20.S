.code16
.section .stage2.util, "ax"

.global has_a20
.global set_a20

has_a20:
    cli
    mov $0x0, %ax
    not %ax
    mov %ax, %fs
    mov %ds:0x7DFE, %ax
    cmp %ax, %fs:0x7E0E
    jne 1f
    ror $0x8, %ax
    mov %ax, %ds:0x7DFE
    cmp %ax, %fs:0x7E0E
    jne 1f
    mov $0x0, %ax
    jmp 2f
1:
    mov $0x1, %ax
2:
    sti
    ret

set_a20:
    call has_a20
    test %ax, %ax
    jnz 3f
    mov $0x2403, %ax
    int $0x15
    jc 1f
    cmp $0, %ah
    jnz 1f
    mov $0x2401, %ax
    int $0x15
    call has_a20
    test %ax, %ax
    jnz 3f
1:
    in $0x92, %al
    test $0x2, %al
    jnz 2f
    or $0x2, %al
    and $0xFE, %al
    out %al, $0x92
    call has_a20
    test %ax, %ax
    jnz 3f
    mov $0x0, %ax
    jmp 3f
2:
    mov $0x1, %ax
3:
    ret
