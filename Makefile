OUTDIR := build

CROSSDIR := ${HOME}/opt/cross
BINDIR := ${CROSSDIR}/bin
TARGETCC := ${BINDIR}/i686-elf-gcc

TARGET := i386-pc

LDFLAGS := -m32 -nostdlib -ffreestanding \
		   -Wl,--no-warn-rwx-segments,--build-id=none,-z,noexecstack

WARNINGS := \
	-Wall -Wextra -Werror -Wpedantic -Wshadow -Wpointer-arith -Wstrict-prototypes \
	-Wmissing-prototypes -Wno-missing-braces -Wno-missing-field-initializers -Wbad-function-cast \
	-Winline -Wundef -Wunreachable-code -Wredundant-decls -Wfloat-equal -Wcast-align \
	-Wcast-qual -Wdeclaration-after-statement -Wmissing-include-dirs -Wnested-externs \
	-Wno-error=format -Wsequence-point -Wswitch -Wwrite-strings -pedantic-errors \
	-Wno-unused

CFLAGS := ${WARNINGS} -Os -g -c -m32 -fno-pie -nostdlib -ffreestanding \
		  -fno-strict-aliasing -mno-red-zone -fstack-protector-all \
		  -falign-jumps=1 -falign-loops=1 -falign-functions=1

BOOT_SRCDIR   := src/boot/${TARGET}
BOOT_SRCFILES := ${wildcard ${BOOT_SRCDIR}/*.S}
BOOT_INCLDIRS := include/boot/${TARGET}
BOOT_OBJFILES := ${patsubst %.S,${OUTDIR}/%.o,${BOOT_SRCFILES}}
BOOT_DEPFILES := ${BOOT_OBJFILES:.o=.d}

KERN_SRCDIR   := src/kern
KERN_SRCFILES := ${wildcard ${KERN_SRCDIR}/*.c}
KERN_INCLDIRS := include/kern include/mods
KERN_OBJFILES := ${patsubst %.c,${OUTDIR}/%.o,${KERN_SRCFILES}}
KERN_DEPFILES := ${KERN_OBJFILES:.o=.d}

MACH_SRCDIR   := ${KERN_SRCDIR}/i386
MACH_SRCFILES := ${wildcard ${MACH_SRCDIR}/*.c}
MACH_INCLDIRS := ${KERN_INCLDIRS}
MACH_OBJFILES := ${patsubst %.c,${OUTDIR}/%.o,${MACH_SRCFILES}}
MACH_DEPFILES := ${MACH_OBJFILES:.o=.d}

MOD_SRCDIR    := src/mods
MOD_SRCFILES  := ${wildcard ${MOD_SRCDIR}/*.c}
MOD_INCLDIRS  := include/kern include/mods
MOD_OBJFILES  := ${patsubst %.c,${OUTDIR}/%.o,${MOD_SRCFILES}}
MOD_DEPFILES  := ${MOD_OBJFILES:.o=.d}

BOOTLD  := ${BOOT_SRCDIR}/boot.ld
BOOTELF := ${OUTDIR}/boot.elf
BOOTIMG := ${OUTDIR}/boot.img

.PHONY: all clean

define USEVAR
${if ${1}, ${1}}
endef

all: ${BOOTIMG}

clean:
	rm -rf ${OUTDIR}

${BOOTIMG}: ${BOOTELF} | ${OUTDIR}
	objcopy -O binary $< $@

${BOOTELF}: ${BOOTLD} ${BOOT_OBJFILES} ${KERN_OBJFILES} ${MACH_OBJFILES} ${MOD_OBJFILES} | ${OUTDIR}
	${TARGETCC} ${LDFLAGS} ${BOOT_OBJFILES} ${KERN_OBJFILES} ${MACH_OBJFILES} ${MOD_OBJFILES} -T ${BOOTLD} -o $@ -lgcc

${OUTDIR}:
	mkdir -p $@

define MKSRCDIR
${OUTDIR}/${1}/%.o: ${1}/%.${2} | ${OUTDIR}/${1}
	${TARGETCC} ${CFLAGS}${call USEVAR,${4}} -MMD ${patsubst %,-I%,${3}} $$< -o $$@
${OUTDIR}/${1}:
	mkdir -p $$@
endef

${eval ${call MKSRCDIR,${BOOT_SRCDIR},S,${BOOT_INCLDIRS}}}
${eval ${call MKSRCDIR,${KERN_SRCDIR},c,${KERN_INCLDIRS},-DROOT_KERNEL}}
${eval ${call MKSRCDIR,${MACH_SRCDIR},c,${MACH_INCLDIRS},-DROOT_KERNEL}}
${eval ${call MKSRCDIR,${MOD_SRCDIR},c,${MOD_INCLDIRS},-DROOT_MODULE -DROOT_KERNEL}}

-include ${BOOT_DEPFILES}
-include ${KERN_DEPFILES}
-include ${MACH_DEPFILES}
-include ${MOD_DEPFILES}