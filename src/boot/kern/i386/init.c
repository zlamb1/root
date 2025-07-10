#include "init.h"
#include "console/font.h"
#include "console/gfx.h"
#include "console/gfx_mode.h"
#include "console/print.h"
#include "console/psf.h"
#include "i386/isr.h"
#include "i386/pic.h"
#include "i386/pit.h"
#include "i386/vbe.h"
#include "i386/vga.h"
#include "machine.h"
#include "memory/alloc.h"
#include "memory/malloc.h"
#include "panic.h"
#include "pci.h"
#include "task.h"
#include "types.h"

extern root_u8 DRIVE_NUMBER;

extern void *root_pit_irq[];
extern void *root_kb_irq[];

void root_clock (void);

void
root_clock (void)
{
  root_pic_eoi (0);
  root_scheduler_tick ();
}

static root_vga_console_t vga_console;
static root_gfx_console_t gfx_console;

root_err_t
root_machine_init (void)
{
  vga_console_init (&vga_console);
  root_initprint (&vga_console.base);
  do
    {
      int mode;
      root_video_mode_t video_mode;
      root_gfx_mode_t gfx_mode;
      root_font_t font;
      if (root_initalloc ())
        root_panic ("failed to init alloc\n");
      if ((mode = root_getvideomode (1920, 1080, 32, &video_mode)) < 0)
        {
          root_printf ("failed to get video mode\n");
          break;
        }
      root_setvideomode (mode);
      gfx_mode = (root_gfx_mode_t){ .width = video_mode.width,
                                    .height = video_mode.height,
                                    .stride = video_mode.pitch,
                                    .bpp = video_mode.bpp >> 3,
                                    .fb = video_mode.framebuffer };
      if (root_psf_get_font (&font) != ROOT_SUCCESS)
        {
          root_printf ("failed to get font\n");
          break;
        }
      gfx_console_init (&gfx_console, &gfx_mode, &font);
      root_initprint (&gfx_console.base);
    }
  while (0);
  root_init_pit (ROOT_PIT_FREQ_MS_DIV);
  root_init_idt ();
  root_set_isr (0x20, &root_pit_irq[0], 0x8E);
  root_set_isr (0x21, &root_kb_irq[0], 0x8E);
  root_load_idt ();
  root_pic_init (0x20, 0x28);
  root_mach_sti ();
  {
    root_pci_headers_t headers;
    if (root_pci_register_devmods () != ROOT_SUCCESS)
      root_panic ("failed to register PCI devmods");
    if (root_pci_enumerate (&headers) != ROOT_SUCCESS)
      root_panic ("failed to enumerate PCI devices");
    root_free (headers.headers);
  }
  return ROOT_SUCCESS;
}
