#include "i386-pc/pic.h"
#include "i386-pc/isr.h"
#include "kern/errno.h"
#include "kern/machine.h"
#include "kern/types.h"

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

extern void *root_irq_stub_table[];
extern irq_handler root_irq_table[];

static int set_pic1_offset = -1, set_pic2_offset = -1;

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
  set_pic1_offset = pic1_offset;
  set_pic2_offset = pic2_offset;
  /* mask all interrupts on init */
  for (int i = 0; i < 16; i++)
    {
      /* don't mask slave irq */
      if (i == 2)
        continue;
      root_pic_mask_irq (i);
    }
  for (int i = 0; i < 8; i++)
    root_set_isr (pic1_offset + i, root_irq_stub_table[i], 0x8E);
  for (int i = 0; i < 8; i++)
    root_set_isr (pic2_offset + i, root_irq_stub_table[i + 8], 0x8E);
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
root_pic_set_isr (root_uint8_t irq, irq_handler handler)
{
  if (irq > 15 || handler == NULL)
    {
      root_seterrno (ROOT_EINVAL);
      return;
    }
  /* FIXME: DISABLE INTERRUPTS! */
  root_irq_table[irq] = handler;
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
