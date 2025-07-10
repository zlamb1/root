ENTRY(start)

SECTIONS
{
    STAGE1_LOC = 0x07C00;
    STAGE1_STK = 0x07C00; 
    STAGE2_LOC = 0x07E00;
    STAGE2_LOAD_LOC = STAGE1_LOC + 0x200 * STAGE2_LBA;
    STAGE2_LBA = 34;
    STAGE2_STK = 0x7FFFF;

    . = STAGE1_LOC;

    .stage1 :
    {
        *(.stage1.text)
    }

    .stage2 STAGE2_LOC : AT(STAGE2_LOAD_LOC)
    {
        *(.stage2.text)
        *(.stage2.utility)
        *(.stage2.data)
        *(.text)
        *(.data)
        *(.rodata)
        _BSS_START = .;
        *(.bss)
        _BSS_END = .;
        *(.eh_frame)
        FILL(0)
        . = ALIGN(512);
        _estage2 = .;
    }

    STAGE2_SEC = (_estage2 - STAGE2_LOC) / 0x200;
    STAGE2_END_LBA = STAGE2_LBA + STAGE2_SEC;

    STAGE2_START_CHS0 = (STAGE2_LBA / 63) % 15;
    STAGE2_START_CHS1 = (STAGE2_LBA % 63) + 1;
    STAGE2_START_CHS2 = (STAGE2_LBA / 63) / 15;

    STAGE2_END_CHS0 = (STAGE2_END_LBA / 63) % 15;
    STAGE2_END_CHS1 = (STAGE2_END_LBA % 63) + 1;
    STAGE2_END_CHS2 = (STAGE2_END_LBA / 63) / 15;
}

OUTPUT_FORMAT(elf32-i386)