#ifndef ROOT_MACHINE_H
#define ROOT_MACHINE_H 1

/*
 * Interface for Generic Bootloader Code to Perform Arch-Specific Tasks.
 * Note: Some Operations May Be No-Ops.
 */

void root_mach_sti (void);
void root_mach_cli (void);
void root_mach_hlt (void);
void root_mach_reboot (void);

void root_mach_store_regs (void);
void root_mach_print_regs (void);

#endif
