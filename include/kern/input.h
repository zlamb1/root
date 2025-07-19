#ifndef ROOT_INPUT_H
#define ROOT_INPUT_H 1

#include "kern/kc.h"
#include "kern/types.h"

#define ROOT_KEY_RELEASE 0
#define ROOT_KEY_PRESS   1
#define ROOT_KEY_REPEAT  2

#define ROOT_SHIFT_MOD    0x1
#define ROOT_ALT_MOD      0x2
#define ROOT_CAPSLOCK_MOD 0x4

typedef root_uint8_t root_keystate_t;

typedef struct
{
  root_keycode_t kc;
  root_keystate_t state;
  root_uint8_t mods;
  char ascii;
} root_input_event_t;

int root_enqueue_input (root_keycode_t kc, root_keystate_t state);
int root_poll_input (root_input_event_t *evt);

#endif
