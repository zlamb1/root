#ifndef ROOT_KC_H
#define ROOT_KC_H 1

#define ROOT_KEY_RESERVED   0x00
#define ROOT_KEY_ESC        0x01
#define ROOT_KEY_F1         0x02
#define ROOT_KEY_F2         0x03
#define ROOT_KEY_F3         0x04
#define ROOT_KEY_F4         0x05
#define ROOT_KEY_F5         0x06
#define ROOT_KEY_F6         0x07
#define ROOT_KEY_F7         0x08
#define ROOT_KEY_F8         0x09
#define ROOT_KEY_F9         0x0A
#define ROOT_KEY_F10        0x0B
#define ROOT_KEY_F11        0x0C
#define ROOT_KEY_F13        0x0D
#define ROOT_KEY_F14        0x0E
#define ROOT_KEY_F15        0x0F
#define ROOT_KEY_F16        0x10
#define ROOT_KEY_F17        0x11
#define ROOT_KEY_F18        0x12
#define ROOT_KEY_F19        0x13
#define ROOT_KEY_F20        0x14
#define ROOT_KEY_F21        0x15
#define ROOT_KEY_F22        0x16
#define ROOT_KEY_F23        0x17
#define ROOT_KEY_F24        0x18
#define ROOT_KEY_GRAVE      0x19
#define ROOT_KEY_1          0x1A
#define ROOT_KEY_2          0x1B
#define ROOT_KEY_3          0x1C
#define ROOT_KEY_4          0x1D
#define ROOT_KEY_5          0x1E
#define ROOT_KEY_6          0x1F
#define ROOT_KEY_7          0x20
#define ROOT_KEY_8          0x21
#define ROOT_KEY_9          0x22
#define ROOT_KEY_0          0x23
#define ROOT_KEY_HYPHEN     0x24
#define ROOT_KEY_EQUALS     0x25
#define ROOT_KEY_BACKSPACE  0x26
#define ROOT_KEY_TAB        0x27
#define ROOT_KEY_Q          0x28
#define ROOT_KEY_W          0x29
#define ROOT_KEY_E          0x2A
#define ROOT_KEY_R          0x2B
#define ROOT_KEY_T          0x2C
#define ROOT_KEY_Y          0x2D
#define ROOT_KEY_U          0x2E
#define ROOT_KEY_I          0x2F
#define ROOT_KEY_O          0x30
#define ROOT_KEY_P          0x31
#define ROOT_KEY_LEFTBRACE  0x32
#define ROOT_KEY_RIGHTBRACE 0x33
#define ROOT_KEY_BACKSLASH  0x34
#define ROOT_KEY_CAPSLOCK   0x35
#define ROOT_KEY_A          0x36
#define ROOT_KEY_S          0x37
#define ROOT_KEY_D          0x38
#define ROOT_KEY_F          0x39
#define ROOT_KEY_G          0x3A
#define ROOT_KEY_H          0x3B
#define ROOT_KEY_J          0x3C
#define ROOT_KEY_K          0x3D
#define ROOT_KEY_L          0x3E
#define ROOT_KEY_SEMICOLON  0x3F
#define ROOT_KEY_APOSTROPHE 0x40
#define ROOT_KEY_ENTER      0x41
#define ROOT_KEY_LEFTSHIFT  0x42
#define ROOT_KEY_Z          0x43
#define ROOT_KEY_X          0x44
#define ROOT_KEY_C          0x45
#define ROOT_KEY_V          0x46
#define ROOT_KEY_B          0x47
#define ROOT_KEY_N          0x48
#define ROOT_KEY_M          0x49
#define ROOT_KEY_COMMA      0x4A
#define ROOT_KEY_PERIOD     0x4B
#define ROOT_KEY_SLASH      0x4C
#define ROOT_KEY_RIGHTSHIFT 0x4D
#define ROOT_KEY_LEFTCTRL   0x4E
#define ROOT_KEY_SUPER      0x4F
#define ROOT_KEY_LEFTALT    0x50
#define ROOT_KEY_SPACE      0x51
#define ROOT_KEY_RIGHTALT   0x52
#define ROOT_KEY_FN         0x53
#define ROOT_KEY_RIGHTCTRL  0x54
#define ROOT_KEY_LEFT       0x55
#define ROOT_KEY_UP         0x56
#define ROOT_KEY_DOWN       0x57
#define ROOT_KEY_RIGHT      0x58
#define ROOT_KEY_PAGEUP     0x59
#define ROOT_KEY_PAGEDOWN   0x5A

typedef unsigned char root_keycode_t;

static const char root_keycode_to_ascii_map[256]
    = { [ROOT_KEY_GRAVE] = '`',       [ROOT_KEY_1] = '1',
        [ROOT_KEY_2] = '2',           [ROOT_KEY_3] = '3',
        [ROOT_KEY_4] = '4',           [ROOT_KEY_5] = '5',
        [ROOT_KEY_6] = '6',           [ROOT_KEY_7] = '7',
        [ROOT_KEY_8] = '8',           [ROOT_KEY_9] = '9',
        [ROOT_KEY_0] = '0',           [ROOT_KEY_HYPHEN] = '-',
        [ROOT_KEY_EQUALS] = '=',      [ROOT_KEY_BACKSPACE] = '\b',
        [ROOT_KEY_TAB] = '\t',        [ROOT_KEY_Q] = 'q',
        [ROOT_KEY_W] = 'w',           [ROOT_KEY_E] = 'e',
        [ROOT_KEY_R] = 'r',           [ROOT_KEY_T] = 't',
        [ROOT_KEY_Y] = 'y',           [ROOT_KEY_U] = 'u',
        [ROOT_KEY_I] = 'i',           [ROOT_KEY_O] = 'o',
        [ROOT_KEY_P] = 'p',           [ROOT_KEY_LEFTBRACE] = '[',
        [ROOT_KEY_RIGHTBRACE] = ']',  [ROOT_KEY_BACKSLASH] = '\\',
        [ROOT_KEY_A] = 'a',           [ROOT_KEY_S] = 's',
        [ROOT_KEY_D] = 'd',           [ROOT_KEY_F] = 'f',
        [ROOT_KEY_G] = 'g',           [ROOT_KEY_H] = 'h',
        [ROOT_KEY_J] = 'j',           [ROOT_KEY_K] = 'k',
        [ROOT_KEY_L] = 'l',           [ROOT_KEY_SEMICOLON] = ';',
        [ROOT_KEY_APOSTROPHE] = '\'', [ROOT_KEY_ENTER] = '\n',
        [ROOT_KEY_Z] = 'z',           [ROOT_KEY_X] = 'x',
        [ROOT_KEY_C] = 'c',           [ROOT_KEY_V] = 'v',
        [ROOT_KEY_B] = 'b',           [ROOT_KEY_N] = 'n',
        [ROOT_KEY_M] = 'm',           [ROOT_KEY_COMMA] = ',',
        [ROOT_KEY_PERIOD] = '.',      [ROOT_KEY_SLASH] = '/',
        [ROOT_KEY_SPACE] = ' ' };

static const char root_mod_keycode_to_ascii_map[256]
    = { [ROOT_KEY_GRAVE] = '~',      [ROOT_KEY_1] = '!',
        [ROOT_KEY_2] = '@',          [ROOT_KEY_3] = '#',
        [ROOT_KEY_4] = '$',          [ROOT_KEY_5] = '%',
        [ROOT_KEY_6] = '^',          [ROOT_KEY_7] = '&',
        [ROOT_KEY_8] = '*',          [ROOT_KEY_9] = '(',
        [ROOT_KEY_0] = ')',          [ROOT_KEY_HYPHEN] = '_',
        [ROOT_KEY_EQUALS] = '+',     [ROOT_KEY_BACKSPACE] = '\b',
        [ROOT_KEY_TAB] = '\t',       [ROOT_KEY_Q] = 'Q',
        [ROOT_KEY_W] = 'W',          [ROOT_KEY_E] = 'E',
        [ROOT_KEY_R] = 'R',          [ROOT_KEY_T] = 'T',
        [ROOT_KEY_Y] = 'Y',          [ROOT_KEY_U] = 'U',
        [ROOT_KEY_I] = 'I',          [ROOT_KEY_O] = 'O',
        [ROOT_KEY_P] = 'P',          [ROOT_KEY_LEFTBRACE] = '{',
        [ROOT_KEY_RIGHTBRACE] = '}', [ROOT_KEY_BACKSLASH] = '|',
        [ROOT_KEY_A] = 'A',          [ROOT_KEY_S] = 'S',
        [ROOT_KEY_D] = 'D',          [ROOT_KEY_F] = 'F',
        [ROOT_KEY_G] = 'G',          [ROOT_KEY_H] = 'H',
        [ROOT_KEY_J] = 'J',          [ROOT_KEY_K] = 'K',
        [ROOT_KEY_L] = 'L',          [ROOT_KEY_SEMICOLON] = ':',
        [ROOT_KEY_APOSTROPHE] = '"', [ROOT_KEY_ENTER] = '\n',
        [ROOT_KEY_Z] = 'Z',          [ROOT_KEY_X] = 'X',
        [ROOT_KEY_C] = 'C',          [ROOT_KEY_V] = 'V',
        [ROOT_KEY_B] = 'B',          [ROOT_KEY_N] = 'N',
        [ROOT_KEY_M] = 'M',          [ROOT_KEY_COMMA] = '<',
        [ROOT_KEY_PERIOD] = '>',     [ROOT_KEY_SLASH] = '?',
        [ROOT_KEY_SPACE] = ' ' };

static int
root_is_key_alpha (root_keycode_t kc)
{
  switch (kc)
    {
    case ROOT_KEY_A:
    case ROOT_KEY_B:
    case ROOT_KEY_C:
    case ROOT_KEY_D:
    case ROOT_KEY_E:
    case ROOT_KEY_F:
    case ROOT_KEY_G:
    case ROOT_KEY_H:
    case ROOT_KEY_I:
    case ROOT_KEY_J:
    case ROOT_KEY_K:
    case ROOT_KEY_L:
    case ROOT_KEY_M:
    case ROOT_KEY_N:
    case ROOT_KEY_O:
    case ROOT_KEY_P:
    case ROOT_KEY_Q:
    case ROOT_KEY_R:
    case ROOT_KEY_S:
    case ROOT_KEY_T:
    case ROOT_KEY_U:
    case ROOT_KEY_V:
    case ROOT_KEY_W:
    case ROOT_KEY_X:
    case ROOT_KEY_Y:
    case ROOT_KEY_Z:
      return 1;
    }
  return 0;
}

#endif
