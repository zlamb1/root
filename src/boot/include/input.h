#ifndef ROOT_INPUT_H
#define ROOT_INPUT_H 1

#include "input/keycode.h"
#include "types.h"

#define ROOT_INPUT_SHIFT_FLAG     0x1
#define ROOT_INPUT_ALT_FLAG       0x2
#define ROOT_INPUT_CAPS_LOCK_FLAG 0x4

typedef enum root_keystate_t
{
  ROOT_KEY_STATE_RELEASE = 0,
  ROOT_KEY_STATE_PRESS = 1,
  ROOT_KEY_STATE_REPEAT = 2,
} root_keystate_t;

typedef struct root_input_t
{
  root_keycode_t keycode;
  root_keystate_t state;
  root_uint8_t flags;
  char ascii;
} root_input_t;

int root_poll_input (root_input_t *input);

#endif
