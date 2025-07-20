#include "i386-pc/pic.h"
#include "kern/input.h"
#include "kern/kc.h"
#include "kern/machine.h"
#include "kern/mod.h"
#include "kern/types.h"

#define PS2_IRQ 1

static char ps2_scanset1_to_key_codes[256] = {
  [0x01] = ROOT_KEY_ESC,        [0x02] = ROOT_KEY_1,
  [0x03] = ROOT_KEY_2,          [0x04] = ROOT_KEY_3,
  [0x05] = ROOT_KEY_4,          [0x06] = ROOT_KEY_5,
  [0x07] = ROOT_KEY_6,          [0x08] = ROOT_KEY_7,
  [0x09] = ROOT_KEY_8,          [0x0A] = ROOT_KEY_9,
  [0x0B] = ROOT_KEY_0,          [0x0C] = ROOT_KEY_HYPHEN,
  [0x0D] = ROOT_KEY_EQUALS,     [0x0E] = ROOT_KEY_BACKSPACE,
  [0x0F] = ROOT_KEY_TAB,        [0x10] = ROOT_KEY_Q,
  [0x11] = ROOT_KEY_W,          [0x12] = ROOT_KEY_E,
  [0x13] = ROOT_KEY_R,          [0x14] = ROOT_KEY_T,
  [0x15] = ROOT_KEY_Y,          [0x16] = ROOT_KEY_U,
  [0x17] = ROOT_KEY_I,          [0x18] = ROOT_KEY_O,
  [0x19] = ROOT_KEY_P,          [0x1A] = ROOT_KEY_LEFTBRACE,
  [0x1B] = ROOT_KEY_RIGHTBRACE, [0x1C] = ROOT_KEY_ENTER,
  [0x1D] = ROOT_KEY_LEFTCTRL,   [0x1E] = ROOT_KEY_A,
  [0x1F] = ROOT_KEY_S,          [0x20] = ROOT_KEY_D,
  [0x21] = ROOT_KEY_F,          [0x22] = ROOT_KEY_G,
  [0x23] = ROOT_KEY_H,          [0x24] = ROOT_KEY_J,
  [0x25] = ROOT_KEY_K,          [0x26] = ROOT_KEY_L,
  [0x27] = ROOT_KEY_SEMICOLON,  [0x28] = ROOT_KEY_APOSTROPHE,
  [0x29] = ROOT_KEY_GRAVE,      [0x2A] = ROOT_KEY_LEFTSHIFT,
  [0x2B] = ROOT_KEY_BACKSLASH,  [0x2C] = ROOT_KEY_Z,
  [0x2D] = ROOT_KEY_X,          [0x2E] = ROOT_KEY_C,
  [0x2F] = ROOT_KEY_V,          [0x30] = ROOT_KEY_B,
  [0x31] = ROOT_KEY_N,          [0x32] = ROOT_KEY_M,
  [0x33] = ROOT_KEY_COMMA,      [0x34] = ROOT_KEY_PERIOD,
  [0x35] = ROOT_KEY_SLASH,      [0x36] = ROOT_KEY_RIGHTSHIFT,
  [0x38] = ROOT_KEY_LEFTALT,    [0x39] = ROOT_KEY_SPACE,
  [0x3A] = ROOT_KEY_CAPSLOCK,   [0x3B] = ROOT_KEY_F1,
  [0x3C] = ROOT_KEY_F2,         [0x3D] = ROOT_KEY_F3,
  [0x3E] = ROOT_KEY_F4,         [0x3F] = ROOT_KEY_F5,
  [0x40] = ROOT_KEY_F6,         [0x41] = ROOT_KEY_F7,
  [0x42] = ROOT_KEY_F8,         [0x43] = ROOT_KEY_F9,
  [0x44] = ROOT_KEY_F10,
};

static volatile root_uint8_t sz = 0;
static volatile root_uint8_t buf[8];

static void
root_recv_sc (void)
{
  root_uint8_t sc = root_inb (0x60);
  root_keycode_t kc;
  if (sz > 0)
    {
      switch (sc)
        {
        case 0x48:
          kc = ROOT_KEY_UP;
          break;
        case 0x4B:
          kc = ROOT_KEY_LEFT;
          break;
        case 0x4D:
          kc = ROOT_KEY_RIGHT;
          break;
        case 0x49:
          kc = ROOT_KEY_PAGEUP;
          break;
        case 0x50:
          kc = ROOT_KEY_DOWN;
          break;
        case 0x51:
          kc = ROOT_KEY_PAGEDOWN;
          break;
        default:
          kc = ROOT_KEY_RESERVED;
          break;
        }
      sz = 0;
    }
  else if (sc == 0xE0)
    {
      sz++;
      buf[0] = 0xE0;
      root_pic_eoi (PS2_IRQ);
      return;
    }
  else
    kc = ps2_scanset1_to_key_codes[sc & ~0x80];
  root_enqueue_input (kc, (sc & 0x80) == 0);
  root_pic_eoi (PS2_IRQ);
}

ROOT_MOD_INIT (ps2)
{
  root_pic_set_isr (PS2_IRQ, root_recv_sc);
  root_pic_unmask_irq (PS2_IRQ);
}

ROOT_MOD_FINI () { root_pic_mask_irq (PS2_IRQ); }
