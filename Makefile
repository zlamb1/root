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
		  -falign-jumps=1 -falign-loops=1 -falign-functions=1 \
		  -mno-mmx -mno-sse -mno-sse2 -mno-3dnow

BOOT_SRCDIR   := src/boot/${TARGET}
BOOT_SRCFILES := ${wildcard ${BOOT_SRCDIR}/*.S}
BOOT_INCLDIRS := include/boot/${TARGET}
BOOT_OBJFILES := ${patsubst %.S,${OUTDIR}/%.o,${BOOT_SRCFILES}}
BOOT_DEPFILES := ${BOOT_OBJFILES:.o=.d}

KERN_SRCDIR   := src/kern
KERN_SRCFILES := ${wildcard ${KERN_SRCDIR}/*.c}
KERN_INCLDIRS := include/
KERN_OBJFILES := ${patsubst %.c,${OUTDIR}/%.o,${KERN_SRCFILES}}
KERN_DEPFILES := ${KERN_OBJFILES:.o=.d}

MACH_NAME     := i386-pc
MACH_SRCDIR   := src/${MACH_NAME}
MACH_SRCFILES := ${wildcard ${MACH_SRCDIR}/*.c}
MACH_INCLDIRS := ${KERN_INCLDIRS}
MACH_OBJFILES := ${patsubst %.c,${OUTDIR}/%.o,${MACH_SRCFILES}}
MACH_DEPFILES := ${MACH_OBJFILES:.o=.d}

MODS_OUTDIR    := ${OUTDIR}/mods
MODS_SRCDIR    := src/mods
MODS_SRCFILES  := ${wildcard ${MODS_SRCDIR}/*.c} ${wildcard ${MODS_SRCDIR}/${MACH_NAME}/*.c}
MODS_INCLDIRS  := include/
MODS_OBJFILES  := ${addprefix ${MODS_OUTDIR}/,${notdir ${MODS_SRCFILES:.c=.mod}}}
MODS_DEPFILES  := ${MODS_OBJFILES:.mod=.d}

MODS_BUILTINS      := ps2
MODS_BUILTINS_OBJS := ${addprefix ${MODS_OUTDIR}/,${addsuffix .o,${MODS_BUILTINS}}}

BOOTLD  := ${BOOT_SRCDIR}/boot.ld
BOOTELF := ${OUTDIR}/boot.elf
BOOTIMG := ${OUTDIR}/boot.img

.PHONY: all clean

define USEVAR
${if ${1}, ${1}}
endef

all: ${BOOTIMG} ${MODS_OBJFILES}

clean:
	rm -rf ${OUTDIR}

${BOOTIMG}: ${BOOTELF} | ${OUTDIR}
	objcopy -O binary $< $@

${BOOTELF}: ${BOOTLD} ${BOOT_OBJFILES} ${KERN_OBJFILES} ${MACH_OBJFILES} ${MODS_BUILTINS_OBJS} | ${OUTDIR}
	${TARGETCC} ${LDFLAGS} ${BOOT_OBJFILES} ${KERN_OBJFILES} ${MACH_OBJFILES} ${MODS_BUILTINS_OBJS} \
		-T ${BOOTLD} -o $@ -lgcc

${MODS_OUTDIR}/%.o: ${MODS_OUTDIR}/%.mod
	cp $< $@

${MODS_OUTDIR}:
	mkdir -p $@

${OUTDIR}:
	mkdir -p $@

define MKSRCDIR
${OUTDIR}/${1}/%.o: ${1}/%.${2} | ${OUTDIR}/${1}
	${TARGETCC} ${CFLAGS}${call USEVAR,${4}} -MMD ${patsubst %,-I%,${3}} $$< -o $$@
${OUTDIR}/${1}:
	mkdir -p $$@
endef

define MKMODRULE
${MODS_OUTDIR}/${notdir ${1:.c=.mod}}: ${1} | ${MODS_OUTDIR}
	${TARGETCC} ${CFLAGS} -DROOT_MODULE -MMD ${patsubst %,-I%,${MODS_INCLDIRS}} $$< -o $$@
endef

${eval ${call MKSRCDIR,${BOOT_SRCDIR},S,${BOOT_INCLDIRS}}}
${eval ${call MKSRCDIR,${KERN_SRCDIR},c,${KERN_INCLDIRS},-DROOT_KERNEL}}
${eval ${call MKSRCDIR,${MACH_SRCDIR},c,${MACH_INCLDIRS},-DROOT_KERNEL}}

${foreach SRCFILE,${MODS_SRCFILES},${eval ${call MKMODRULE,${SRCFILE}}}}

-include ${BOOT_DEPFILES}
-include ${KERN_DEPFILES}
-include ${MACH_DEPFILES}
-include ${MODS_DEPFILES}