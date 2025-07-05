#include "machine.h"
#include "console/print.h"

typedef struct i386_regs_t
{
  int eax;
  int ebx;
  int ecx;
  int edx;
  int esi;
  int edi;
  int ebp;
  int esp;
  int eflags;
} i386_regs_t;

static i386_regs_t regs;

void
root_mach_sti (void)
{
  __asm__ volatile ("sti" : : : "memory");
}

void
root_mach_cli (void)
{
  __asm__ volatile ("cli" : : : "memory");
}

void
root_mach_hlt (void)
{
  __asm__ volatile ("cli" : : : "memory");
}

void
root_mach_store_regs (void)
{
  __asm__ volatile ("mov %%eax, %0" : "=m"(regs.eax));
  __asm__ volatile ("mov %%ebx, %0" : "=m"(regs.ebx));
  __asm__ volatile ("mov %%ecx, %0" : "=m"(regs.ecx));
  __asm__ volatile ("mov %%edx, %0" : "=m"(regs.edx));
  __asm__ volatile ("mov %%esi, %0" : "=m"(regs.esi));
  __asm__ volatile ("mov %%edi, %0" : "=m"(regs.edi));
  __asm__ volatile ("mov %%ebp, %0" : "=m"(regs.ebp));
  __asm__ volatile ("mov %%esp, %0" : "=m"(regs.esp));
  __asm__ volatile ("pushf; pop %%eax; mov %%eax, %0"
                    : "=m"(regs.eflags)
                    :
                    : "eax");
}

void
root_mach_print_regs (void)
{
  root_printf ("EAX    : %i [%#x]\n", regs.eax, regs.eax);
  root_printf ("EBX    : %i [%#x]\n", regs.ebx, regs.ebx);
  root_printf ("ECX    : %i [%#x]\n", regs.ecx, regs.ecx);
  root_printf ("EDX    : %i [%#x]\n", regs.edx, regs.edx);
  root_printf ("ESI    : %i [%#x]\n", regs.esi, regs.esi);
  root_printf ("EDI    : %i [%#x]\n", regs.edi, regs.edi);
  root_printf ("EBP    : %i [%#x]\n", regs.ebp, regs.ebp);
  root_printf ("ESP    : %i [%#x]\n", regs.esp, regs.esp);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat"
  root_printf ("EFLAGS : %#b\n", regs.eflags);
#pragma GCC diagnostic pop
}
