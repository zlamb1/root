#include "i386/pic.h"
#include "i386/machine_io.h"
#include "types.h"

#define PIC1     0x20
#define PIC2     0xA0
#define PIC1_CMD PIC1
#define PIC1_DTA (PIC1 + 1)
#define PIC2_CMD PIC2
#define PIC2_DTA (PIC2 + 1)

#define PIC_EOI 0x20

#define ICW1_ICW4      0x01
#define ICW1_SINGLE    0x02
#define ICW1_INTERVAL4 0x04
#define ICW1_LEVEL     0x08
#define ICW1_INIT      0x10

#define ICW4_8086       0x01
#define ICW4_AUTO       0x02
#define ICW4_BUF_SLAVE  0x08
#define ICW4_SFNM       0x10
#define ICW4_BUF_MASTER 0x0C

void
root_pic_init (root_uint8_t pic1_offset, root_uint8_t pic2_offset)
{
  root_outb (PIC1_CMD, ICW1_INIT | ICW1_ICW4);
  root_outb (PIC2_CMD, ICW1_INIT | ICW1_ICW4);
  root_outb (PIC1_DTA, pic1_offset);
  root_outb (PIC2_DTA, pic2_offset);
  root_outb (PIC1_DTA, 4);
  root_outb (PIC2_DTA, 2);
  root_outb (PIC1_DTA, ICW4_8086);
  root_outb (PIC2_DTA, ICW4_8086);
  root_outb (PIC1_DTA, 0);
  root_outb (PIC2_DTA, 0);
}

void
root_pic_mask_irq (root_uint8_t irq)
{
  root_uint16_t port;
  root_uint8_t value;
  if (irq < 8)
    port = PIC1_DTA;
  else
    {
      port = PIC2_DTA;
      irq %= 8;
    }
  value = root_inb (port) | (1 << irq);
  root_outb (port, value);
}

void
root_pic_unmask_irq (root_uint8_t irq)
{
  root_uint16_t port;
  root_uint8_t value;
  if (irq < 8)
    port = PIC1_DTA;
  else
    {
      port = PIC2_DTA;
      irq %= 8;
    }
  value = root_inb (port) & ~(1 << irq);
  root_outb (port, value);
}

void
root_pic_eoi (root_uint8_t irq)
{
  if (irq >= 8)
    root_outb (PIC2_CMD, PIC_EOI);
  root_outb (PIC1_CMD, PIC_EOI);
}

void
root_pic_disable (void)
{
  root_outb (PIC1_DTA, 0xFF);
  root_outb (PIC2_DTA, 0xFF);
}
