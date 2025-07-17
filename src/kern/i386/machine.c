#include "machine.h"

void
root_sti (void)
{
  __asm__ volatile ("sti" ::: "memory");
}

void
root_cli (void)
{
  __asm__ volatile ("cli" ::: "memory");
}

void
root_halt (void)
{
  root_cli ();
  for (;;)
    __asm__ volatile ("hlt" ::: "memory");
}

root_uint8_t
root_inb (root_uint16_t port)
{
  root_uint8_t val;
  __asm__ volatile ("inb %w1, %b0" : "=a"(val) : "Nd"(port) : "memory");
  return val;
}

void
root_outb (root_uint16_t port, root_uint8_t val)
{
  __asm__ volatile ("outb %b0, %w1" : : "a"(val), "Nd"(port) : "memory");
}

root_uint16_t
root_inw (root_uint16_t port)
{
  root_uint16_t val;
  __asm__ volatile ("inw %w1, %w0" : "=a"(val) : "Nd"(port) : "memory");
  return val;
}

void
root_outw (root_uint16_t port, root_uint16_t val)
{
  __asm__ volatile ("outw %w0, %w1" : : "a"(val), "Nd"(port) : "memory");
}

root_uint32_t
root_inl (root_uint16_t port)
{
  root_uint32_t val;
  __asm__ volatile ("inl %w1, %k0" : "=a"(val) : "Nd"(port) : "memory");
  return val;
}

void
root_outl (root_uint16_t port, root_uint32_t val)
{
  __asm__ volatile ("outl %k0, %w1" : : "a"(val), "Nd"(port) : "memory");
}
