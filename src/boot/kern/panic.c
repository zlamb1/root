#include "panic.h"
#include "console/print.h"
#include "machine.h"
#include "string.h"

static const int line_size = 64;
static const char panic_msg[]
    = " ________    ________      ________       ___      ________     "
      "|\\   __  \\  |\\   __  \\    |\\   ___  \\    |\\  \\    |\\   "
      "____\\    "
      "\\ \\  \\|\\  \\ \\ \\  \\|\\  \\   \\ \\  \\\\ \\  \\   \\ \\  \\   "
      "\\ \\  \\___|    "
      " \\ \\   ____\\ \\ \\   __  \\   \\ \\  \\\\ \\  \\   \\ \\  \\   \\ "
      "\\  \\       "
      "  \\ \\  \\___|  \\ \\  \\ \\  \\   \\ \\  \\\\ \\  \\   \\ \\  \\   "
      "\\ \\  \\____  "
      "   \\ \\__\\      \\ \\__\\ \\__\\   \\ \\__\\\\ \\__\\   \\ \\__\\   "
      "\\ \\_______\\"
      "    \\|__|       \\|__|\\|__|    \\|__| \\|__|    \\|__|    "
      "\\|_______|";

static void
printpanic (const char *msg)
{
  int width = root_console->width, hwidth = width >> 1;
  int start = hwidth - line_size / 2;
  int msg_size = root_strlen (msg);
  root_printf ("\n");
  for (unsigned int i = 0; i < sizeof (panic_msg) / line_size; i++)
    {
      for (int j = 0; j < start; j++)
        root_putchar_unsynced (' ');
      root_printf ("%.*s\n", line_size, panic_msg + line_size * i);
    }
  root_printf ("\n\n\n");
  for (int i = 0; i < hwidth - msg_size / 2; i++)
    root_putchar_unsynced (' ');
  root_printf ("%s\n", msg);
}

void
root_panic (const char *msg)
{
  root_mach_store_regs ();
  root_mach_cli ();
  root_console->clear (root_console, 0);
  root_console->setcursor (root_console, 0);
  printpanic (msg);
  root_printf ("\n");
  root_mach_print_regs ();
  for (;;)
    root_mach_hlt ();
}
