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
    }

    .initmods :
    {
        *(.rootmod.name)
        *(.rootmod.deps)
        . = ALIGN(4);
        KERN_INITMODS_START = .;
        *(.rootmod.desc)
        KERN_INITMODS_END = .;
    }

    .text :
    {
        *(.text)
    }

    .data :
    {
        *(.data)
    }

    .rodata :
    {
        *(.rodata)
        *(.rodata.*)
    }

    .bss :
    {
        KERN_BSS_START = .;
        *(.bss)
        KERN_BSS_END = .; 
    }

    .eh_frame :
    {
        *(.eh_frame)
    }

    BOOT_STAGE2_END = .;

    KERN_SECTORS = (BOOT_STAGE2_END - BOOT_STAGE2_START) / 0x200;
}

OUTPUT_FORMAT(elf32-i386)