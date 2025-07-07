#include "i386/pit.h"
#include "i386/machine_io.h"

#define PIT_CHANNEL_0_DATA 0x40
#define PIT_CHANNEL_1_DATA 0x41
#define PIT_CHANNEL_2_DATA 0x42
#define PIT_CMD_REGISTER   0x43

void
root_init_pit (root_u16 freq_divider)
{
  root_outb (PIT_CMD_REGISTER,
             ROOT_PIT_CHANNEL_0 | ROOT_PIT_ACCESS_LOBYTE_HIBYTE
                 | ROOT_PIT_OP_MODE_3 | ROOT_PIT_BINARY_MODE);
  root_outb (PIT_CHANNEL_0_DATA, freq_divider & 0xFFFF);
  root_outb (PIT_CHANNEL_0_DATA, freq_divider >> 8);
}
