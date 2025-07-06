SRCDIR := src
BUILDDIR := build
CROSSDIR := ${HOME}/opt/cross
BINDIR := ${CROSSDIR}/bin
TARGET32 := i686-elf

LD32 := ${BINDIR}/${TARGET32}-ld
CC32 := ${BINDIR}/${TARGET32}-gcc
CC := ${CC32}

WARNINGS := \
	-Wall -Wextra -Werror -Wpedantic -Wshadow -Wpointer-arith -Wstrict-prototypes \
	-Wmissing-prototypes -Wno-missing-braces -Wno-missing-field-initializers -Wbad-function-cast \
	-Winline -Wundef -Wunreachable-code -Wredundant-decls -Wfloat-equal -Wcast-align \
	-Wcast-qual -Wdeclaration-after-statement -Wmissing-include-dirs -Wnested-externs \
	-Wno-error=format -Wsequence-point -Wswitch -Wwrite-strings -pedantic-errors \
	-Wno-unused
CFLAGS := \
	${WARNINGS} -g -c -m32 -fno-pie -nostdlib -ffreestanding \
	-fno-strict-aliasing -mno-red-zone -fstack-protector-all
LDFLAGS := \
	-m32 -nostdlib -ffreestanding \
	-Wl,--no-warn-rwx-segments,--build-id=none,-z,noexecstack
LIBFLAGS := -lgcc

BOOTDIR := ${SRCDIR}/boot

BOOT_TARGET := pc
BOOT_TARGET_SRCDIR := ${BOOTDIR}/${BOOT_TARGET}
BOOT_TARGET_SRCFILES := ${wildcard ${BOOT_TARGET_SRCDIR}/*.S}
BOOT_TARGET_OBJFILES := ${patsubst %.S,${BUILDDIR}/%.o,${BOOT_TARGET_SRCFILES}}
BOOT_TARGET_LDFILE := ${BOOT_TARGET_SRCDIR}/boot.ld

BOOT_INCLUDEDIRS := ${BOOTDIR}/include
BOOT_SRCFILES += ${shell find ${BOOTDIR}/kern -name "*.c"}
BOOT_OBJFILES := ${patsubst %.c,${BUILDDIR}/%.o,${BOOT_SRCFILES}}
BOOT_DEPFILES := ${BOOT_OBJFILES:.o=.d}
BOOTLD := ${BUILDDIR}/boot.ld
BOOTELF := ${BUILDDIR}/boot.elf
BOOTBIN := ${BUILDDIR}/boot.bin

FONTSRC := font/Lat2-Terminus20x10.psf
FONTOBJ := ${BUILDDIR}/font.o

.PHONY: all clean font

all: ${BOOTBIN}

usb: ${BOOTBIN}
	@echo "\033[31;1mWARNING:\033[0m; this will clobber /dev/sdb! Edit sdX to the proper USB device before usage."
	sudo dd if=${BOOTBIN} of=/dev/sdb bs=8M

${FONTOBJ}: ${FONTSRC}
	objcopy -O elf32-i386 -B i386 -I binary $< $@

${BOOTBIN}: ${BOOTELF}
	objcopy -O binary $< $@

${BOOTELF}: ${BOOT_TARGET_OBJFILES} ${BOOT_OBJFILES} ${FONTOBJ} ${BOOTLD}
	${CC} ${LDFLAGS} ${FONTOBJ} ${BOOT_TARGET_OBJFILES} ${BOOT_OBJFILES} -T ${BOOTLD} -o $@ ${LIBFLAGS}

${BOOTLD}: ${BOOT_TARGET_LDFILE}
	ln -sf ${realpath $<} $@

define MKTARGETDIR
${BUILDDIR}/${1}/%.o: ${1}/%.${2} | ${BUILDDIR}/${1}
	${CC} ${CFLAGS} -MMD ${patsubst %,-I%,${BOOT_INCLUDEDIRS}} $$< -o $$@
${BUILDDIR}/${1}:
	mkdir -p $$@
endef

${eval ${call MKTARGETDIR,${BOOT_TARGET_SRCDIR},S}}
${eval ${call MKTARGETDIR,${BOOTDIR}/kern,c}}
${eval ${call MKTARGETDIR,${BOOTDIR}/kern/cmd,c}}
${eval ${call MKTARGETDIR,${BOOTDIR}/kern/console,c}}
${eval ${call MKTARGETDIR,${BOOTDIR}/kern/disk,c}}
${eval ${call MKTARGETDIR,${BOOTDIR}/kern/i386,c}}
${eval ${call MKTARGETDIR,${BOOTDIR}/kern/memory,c}}

clean:
	rm -rf ${BUILDDIR}

-include ${BOOT_DEPFILES}