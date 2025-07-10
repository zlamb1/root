#ifndef ROOT_PIT_H
#define ROOT_PIT_H 1

#include "types.h"

#define ROOT_PIT_BASE_FREQ      1193182
#define ROOT_PIT_FREQ_DIV_65536 0
#define ROOT_PIT_FREQ_MS_DIV    (ROOT_PIT_BASE_FREQ / 1000)

#define ROOT_PIT_BINARY_MODE          0x0
#define ROOT_PIT_BCD_MODE             0x1
#define ROOT_PIT_OP_MODE_0            (0x0 << 1) // Interrupt on Terminal Count
#define ROOT_PIT_OP_MODE_1            (0x1 << 1) // Hardware Re-triggerable One Shot
#define ROOT_PIT_OP_MODE_2            (0x2 << 1) // Rate Generator
#define ROOT_PIT_OP_MODE_3            (0x3 << 1) // Square Wave Generator
#define ROOT_PIT_OP_MODE_4            (0x4 << 1) // Software Triggered Strobe
#define ROOT_PIT_OP_MODE_5            (0x5 << 1) // Hardware Triggered Strobe
#define ROOT_PIT_OP_MODE_6            (0x6 << 1) // Rate Generator (same as 2)
#define ROOT_PIT_OP_MODE_7            (0x7 << 1) // Square Wave Generator (same as 3)
#define ROOT_PIT_COUNTER_LATCH_CMD    (0x0 << 4)
#define ROOT_PIT_ACCESS_LOBYTE_ONLY   (0x1 << 4)
#define ROOT_PIT_ACCESS_HIBYTE_ONLY   (0x2 << 4)
#define ROOT_PIT_ACCESS_LOBYTE_HIBYTE (0x3 << 4)
#define ROOT_PIT_CHANNEL_0            (0x0 << 6)
#define ROOT_PIT_CHANNEL_1            (0x1 << 6)
#define ROOT_PIT_CHANNEL_2            (0x2 << 6)
#define ROOT_PIT_READBACK             (0x3 << 6)

void root_init_pit (root_uint16_t freq_divider);

#endif
