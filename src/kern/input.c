#include "kern/input.h"
#include "common/page.h"
#include "kern/alloc.h"
#include "kern/kc.h"
#include "kern/types.h"

typedef struct
{
  root_keycode_t kc;
  root_keycode_t st;
} root_queued_input_t;

static const char *conv[2]
    = { root_keycode_to_ascii_map, root_mod_keycode_to_ascii_map };

static int cap = 0, head = 0, tail = 0;
static root_queued_input_t *buf = NULL;

static int shifts[2] = { 0 };
static int alts[2] = { 0 };
static int caps_lock = 0;

static inline int
next (int n)
{
  return (n + 1) % cap;
}

int
root_enqueue_input (root_keycode_t kc, root_keystate_t st)
{
  int tail_next;
  if (buf == NULL)
    {
      /* TODO: move this out of here; make this module init code */
      buf = root_alloc_page ();
      if (buf == NULL)
        return ROOT_EALLOC;
      cap = ROOT_PAGE_SIZE / sizeof (root_queued_input_t);
    }
  if (!cap || (tail_next = next (tail)) == head)
    return ROOT_ENOENT;
  buf[tail].kc = kc;
  buf[tail].st = st;
  tail = tail_next;
  return ROOT_SUCCESS;
}

int
root_poll_input (root_input_event_t *evt)
{
  int index = 0;
  root_queued_input_t input;

  if (tail == head)
    return 0;

  if (evt == NULL)
    {
      tail = next (tail);
      return 1;
    }

  input = buf[head];
  evt->kc = input.kc;
  evt->state = input.st;
  evt->mods = 0;

  switch (evt->kc)
    {
    case ROOT_KEY_LEFTSHIFT:
      shifts[0] = evt->state;
      break;
    case ROOT_KEY_RIGHTSHIFT:
      shifts[1] = evt->state;
      break;
    case ROOT_KEY_LEFTALT:
      alts[0] = evt->state;
      break;
    case ROOT_KEY_RIGHTALT:
      alts[1] = evt->state;
      break;
    case ROOT_KEY_CAPSLOCK:
      if (evt->state == ROOT_KEY_PRESS)
        caps_lock = !caps_lock;
      break;
    }

  if (shifts[0] || shifts[1])
    {
      evt->mods |= ROOT_SHIFT_MOD;
      index++;
    }

  if (alts[0] || alts[1])
    evt->mods |= ROOT_ALT_MOD;

  if (caps_lock)
    {
      evt->mods |= ROOT_CAPSLOCK_MOD;
      if (root_is_key_alpha (evt->kc))
        index++;
    }

  evt->ascii = conv[index % 2][evt->kc];

  head = next (head);
  return 1;
}
