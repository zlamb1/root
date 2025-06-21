#include "alloc.h"
#include "gfx.h"
#include "mode.h"
#include "print.h"
#include "string.h"

extern int random (void);

static void root_printwelcome (void);
static void root_initcmd (void);
__attribute__ ((noreturn)) void root_main (void);

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
  int width = root_term->getwidth (root_term), hwidth = width >> 1;
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
root_initcmd (void)
{
  // kprintf ("zOS> ");
}

root_gfx_term_t gfx_term;

void
root_main (void)
{
  root_initprint (NULL);
  root_printwelcome ();
  (void) gfx_term;
  if (root_initalloc ())
    {
      root_printf ("failed to init alloc\n");
    }
  do
    {
      int mode;
      root_video_mode_t video_mode;
      if ((mode = root_getvideomode (1280, 800, 32, &video_mode)) < 0)
        {
          root_printf ("failed to get video mode\n");
          break;
        }
      root_setvideomode (mode);
      root_initgfx_term (&gfx_term, &video_mode);
      root_initprint (&gfx_term.base);
      root_printwelcome ();
    }
  while (0);
  (void) root_initcmd;
  for (;;)
    ;
}
