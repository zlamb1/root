#include "i386/vbe.h"
#include "i386/bios.h"
#include "types.h"

#define MAX(A, B) ((A) < (B) ? (B) : (A))
#define MIN(A, B) ((A) < (B) ? (A) : (B))

typedef struct root_vbe_info_t
{
  char sig[4];
  root_u16 ver;
  const char *oem;
  root_u32 capabilities;
  root_u16 modes_offset;
  root_u16 modes_segment;
  root_u16 video_memory;
  root_u16 oem_ver;
  root_u32 vendor_name;
  root_u32 product_name;
  root_u32 product_revision;
  root_u16 vbe_ver;
  root_u32 accelerated_modes;
  char reserved[472];
} __attribute__ ((packed)) root_vbe_info_t;

static void
root_query_mode_info (root_video_mode_t *video_mode, root_u16 mode)
{
  root_bios_args_t args;
  root_uintptr_t address = (root_uintptr_t) video_mode;
  args.eax = 0x4F01;
  args.ecx = mode;
  args.es = root_get_segment (address);
  args.edi = root_get_offset (address);
  root_bios_interrupt (0x10, &args);
}

int
root_vbe_find_best_videomode (root_u16 width, root_u16 height, root_u16 depth,
                              root_video_mode_t *video_mode)
{
  root_bios_args_t args;
  root_vbe_info_t info;
  root_u16 *modes;
  root_size_t best_diff = SIZE_MAX;
  root_uintptr_t address = (root_uintptr_t) &info;
  root_video_mode_t tmp;
  int best_mode = -1;
  args.eax = 0x4F00;
  args.es = root_get_segment (address);
  args.edi = root_get_offset (address);
  root_bios_interrupt (0x10, &args);
  modes
      = (root_u16 *) root_get_pointer (info.modes_segment, info.modes_offset);
  while (*modes != 0xFFFF)
    {
      root_u16 mode = *modes++;
      root_query_mode_info (&tmp, mode);
      if ((tmp.attributes & 0x90) == 0x90
          && (tmp.memory_model == 4 || tmp.memory_model == 6))
        {
          root_size_t mode_diff
              = (MAX (width, tmp.width) - MIN (width, tmp.width))
                + (MAX (height, tmp.height) - MIN (height, tmp.height))
                + (MAX (depth, tmp.bpp) - MIN (depth, tmp.bpp));
          if (!mode_diff)
            {
              *video_mode = tmp;
              return mode;
            }
          if (mode_diff < best_diff)
            {
              best_diff = mode_diff;
              best_mode = mode;
              *video_mode = tmp;
            }
        }
    }
  return best_mode;
}

void
root_vbe_set_videomode (root_u16 mode)
{
  root_bios_args_t args;
  args.eax = 0x4F02;
  args.ebx = mode;
  root_bios_interrupt (0x10, &args);
}
