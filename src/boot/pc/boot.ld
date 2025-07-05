ENTRY(start)

SECTIONS
{
    STAGE1_LOC = 0x07C00;
    STAGE1_STK = 0x07C00; 
    STAGE2_LOC = 0x07E00;
    STAGE2_STK = 0x7FFFF;

    . = STAGE1_LOC;

    .stage1 :
    {
        *(.stage1.text)
    }

    .stage2 :
    {
        *(.stage2.text)
        *(.stage2.utility)
        *(.stage2.data)
        *(.text)
        *(.data)
        *(.rodata)
        *(.bss)
        *(.eh_frame)
        FILL(0)
        . = ALIGN(512);
        _estage2 = .;
    }

    STAGE2_SEC = (_estage2 - STAGE2_LOC) / 0x200;
}

OUTPUT_FORMAT(elf32-i386)