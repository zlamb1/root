ENTRY(start)

SECTIONS
{
    BOOT_STAGE1_START = 0x7C00;
    BOOT_STAGE2_START = 0x7E00;

    . = BOOT_STAGE1_START;

    .stage1 :
    {
        *(.stage1)
    }

    . = BOOT_STAGE2_START;

    .stage2 :
    {
        *(.stage2)
        *(.stage2.*)
        FILL(0)
        *(.rootmod.name)
        *(.rootmod.deps)
        . = ALIGN(4);
        KERN_INITMODS_START = .;
        *(.rootmod.desc)
        KERN_INITMODS_END = .;
        . = ALIGN(512);
        *(.text)
        . = ALIGN(4);
        *(.data)
        . = ALIGN(4);
        *(.rodata)
        *(.rodata.*)
        . = ALIGN(4);
        KERN_BSS_START = .;
        *(.bss)
        KERN_BSS_END = .; 
        *(.eh_frame)
        FILL(0)
        . = ALIGN(512);
        BOOT_STAGE2_END = .;
    }

    KERN_SECTORS = (BOOT_STAGE2_END - BOOT_STAGE2_START) / 0x200;
}

OUTPUT_FORMAT(elf32-i386)