#include "input.h"
#include "i386/machine_io.h"
#include "i386/pic.h"
#include "input/keycode.h"

#define INPUT_BUFFER_MAX 128

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

void root_recv_sc (void);

static volatile root_uint8_t head = 0, tail = 0;
static volatile root_uint8_t buf[INPUT_BUFFER_MAX];

static int shifts[2] = { 0 };
static int alts[2] = { 0 };
static int caps_lock = 0;

static inline root_uint8_t
_next (root_uint8_t n)
{
  return (n + 1) % INPUT_BUFFER_MAX;
}

void
root_recv_sc (void)
{
  root_uint8_t scancode = root_inb (0x60);
  root_uint8_t tail_next = _next (tail);
  if (tail_next == head)
    return;
  buf[tail] = scancode;
  tail = tail_next;
  root_pic_eoi (1);
}

int
root_poll_input (root_input_t *input)
{
  static const char *conv[2]
      = { root_key_code_to_ascii_map, root_key_code_to_ascii_map_alt };
  int index = 0;
  root_uint8_t sc;

  if (tail == head)
    return 0;

  sc = buf[head];

  if (sc == 0xE0)
    {
      root_uint8_t next = _next (head);
      if (next == tail)
        return 0;
      sc = buf[next];
      switch (sc)
        {
        case 0x48:
          input->keycode = ROOT_KEY_UP;
          break;
        case 0x4B:
          input->keycode = ROOT_KEY_LEFT;
          break;
        case 0x4D:
          input->keycode = ROOT_KEY_RIGHT;
          break;
        case 0x49:
          input->keycode = ROOT_KEY_PAGEUP;
          break;
        case 0x50:
          input->keycode = ROOT_KEY_DOWN;
          break;
        case 0x51:
          input->keycode = ROOT_KEY_PAGEDOWN;
          break;
        default:
          input->keycode = ROOT_KEY_RESERVED;
          break;
        }
      input->state = (sc & 0x80) == 0;
      input->ascii = 0;
      head = _next (head);
      return 1;
    }

  input->keycode = ps2_scanset1_to_key_codes[sc & ~0x80];
  input->state = (sc & 0x80) == 0;
  input->flags = 0;

  if (shifts[0] || shifts[1])
    {
      input->flags |= ROOT_INPUT_SHIFT_FLAG;
      index++;
    }

  if (alts[0] || alts[1])
    input->flags |= ROOT_INPUT_ALT_FLAG;

  if (caps_lock)
    {
      input->flags |= ROOT_INPUT_CAPS_LOCK_FLAG;
      if (root_is_key_code_alpha (input->keycode))
        index++;
    }

  input->ascii = conv[index % 2][input->keycode];

  switch (input->keycode)
    {
    case ROOT_KEY_LEFTSHIFT:
      shifts[0] = input->state;
      break;
    case ROOT_KEY_RIGHTSHIFT:
      shifts[1] = input->state;
      break;
    case ROOT_KEY_LEFTALT:
      alts[0] = input->state;
      break;
    case ROOT_KEY_RIGHTALT:
      alts[1] = input->state;
      break;
    case ROOT_KEY_CAPSLOCK:
      // TODO: implement PS/2 driver to set LEDs
      if (input->state == ROOT_KEY_STATE_PRESS)
        caps_lock = !caps_lock;
      break;
    }

  head = _next (head);
  return 1;
}
