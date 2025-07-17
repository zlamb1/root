.section .stage2.util, "ax"

.global gdtdesc

gdt:
    # null descriptor
    .quad 0x0
    protcode:
        # limit
        .short 0xFFFF
        # bits 1 - 16 of base address
        .short 0x0
        # bits 17 - 24 of base address
        .byte 0x0
        # type, privilege level, and present flag
        .byte 0b10011010
        # bits 17 - 20 of limit, attributes, granularity
        .byte 0b11001111
        # bits 24 - 32 of base address
        .byte 0x0
    protdata:
        # limit
        .short 0xFFFF
        # bits 1 - 16 of base address
        .short 0x0
        # bits 17 - 24 of base address
        .byte 0x0
        # type, privilege level, and present flag
        .byte 0b10010010
        # bits 17 - 20 of limit, attributes, granularity
        .byte 0b11001111
        # bits 24 - 32 of base address
        .byte 0x0
    realcode:
        .short 0xFFFF, 0x00
        .byte  0x0000, 0x9E, 0x0, 0x0
    realdata:
        .short 0xFFFF, 0x00
        .byte  0x0000, 0x92, 0x0, 0x0
gdtend:

gdtdesc:
    .short gdtend - gdt - 1
    .long  gdt
