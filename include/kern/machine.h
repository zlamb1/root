#ifndef ROOT_MACHINE_H
#define ROOT_MACHINE_H

#include "types.h"

void root_machine_init (void);

void root_sti (void);
void root_cli (void);

void __attribute__ ((noreturn)) root_halt (void);
void root_reboot (void);

root_uint8_t root_inb (root_uint16_t port);
void root_outb (root_uint16_t port, root_uint8_t val);

root_uint16_t root_inw (root_uint16_t port);
void root_outw (root_uint16_t port, root_uint16_t val);

root_uint32_t root_inl (root_uint16_t port);
void root_outl (root_uint16_t port, root_uint32_t val);

#endif
