#include "console/welcome.h"
#include "console/print.h"
#include "string.h"

const int line_size = 46;
const char main_theme[] = ",-.----.                              ___     "
                          "\\    /  \\                           ,--.'|_   "
                          ";   :    \\     ,---.      ,---.     |  | :,'  "
                          "|   | .\\ :    '   ,'\\    '   ,'\\    :  : ' :  "
                          ".   : |: |   /   /   |  /   /   | .;__,'  /   "
                          "|   |  \\ :  .   ; ,. : .   ; ,. : |  |   |    "
                          "|   : .  /  '   | |: : '   | |: : :__,'| :    "
                          ";   | |  \\  '   | .; : '   | .; :   '  : |__  "
                          "|   | ;\\  \\ |   :    | |   :    |   |  | '.'| "
                          ":   ' | \\.'  \\   \\  /   \\   \\  /    ;  :    ; "
                          ":   : :-'     `----'     `----'     |  ,   /  "
                          "|   |.'                              ---`-'   "
                          "`---'                                         ";

const char *greeting = "Welcome to Root.";

void
root_printwelcome (void)
{
  int width = root_console->width, hwidth = width >> 1;
  int start = hwidth - line_size / 2;
  int greeting_size = root_strlen (greeting);
  root_printf ("\n");
  for (unsigned int i = 0; i < sizeof (main_theme) / line_size; i++)
    {
      for (int j = 0; j < start; j++)
        root_putchar_unsynced (' ');
      root_printf ("%.*s\n", line_size, main_theme + line_size * i);
    }
  for (int i = 0; i < hwidth - greeting_size / 2; i++)
    root_putchar_unsynced (' ');
  root_printf ("%s\n\n", greeting);
}

void
root_cmd_welcome (int argc, char **argv)
{
  int clr = 0;
  for (int i = 0; i < argc - 1; i++)
    {
      char *arg = argv[i + 1];
      if (root_strcmp (arg, "-c") == 0)
        clr = 1;
    }
  if (clr)
    root_console->clear (root_console, root_console->bg);
  root_printwelcome ();
}
