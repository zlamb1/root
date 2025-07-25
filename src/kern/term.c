#include "kern/term.h"
#include "common/page.h"
#include "kern/errno.h"
#include "kern/malloc.h"
#include "kern/print.h"
#include "kern/task.h"

#define TERM_DEFAULT_FG ROOT_TERM_COLOR_WHITE
#define TERM_DEFAULT_BG ROOT_TERM_COLOR_BLACK

#define MIN(X, Y) ((X) > (Y) ? (Y) : (X))

typedef enum
{
  ROOT_TERM_STATE_WRI = 0,
  ROOT_TERM_STATE_ESC,
  ROOT_TERM_STATE_CSI
} root_term_state_t;

typedef enum
{
  ROOT_TERM_CSI_CUU = 'A',
  ROOT_TERM_CSI_CUD = 'B',
  ROOT_TERM_CSI_CUF = 'C',
  ROOT_TERM_CSI_CUB = 'D',
  ROOT_TERM_CSI_CNL = 'E',
  ROOT_TERM_CSI_CPL = 'F',
  ROOT_TERM_CSI_CHA = 'G',
  ROOT_TERM_CSI_CUP = 'H',
  ROOT_TERM_CSI_ED = 'J',
  ROOT_TERM_CSI_EL = 'K',
  ROOT_TERM_CSI_SU = 'S',
  ROOT_TERM_CSI_SD = 'T',
  ROOT_TERM_CSI_HVP = 'f',
  ROOT_TERM_CSI_SGR = 'm',
  ROOT_TERM_CSI_SCP = 's',
  ROOT_TERM_CSI_RCP = 'u'
} root_term_csi_t;

static root_term_t *terms = NULL;
static root_term_t *primary_term = NULL;

static unsigned char vt_color_map[8]
    = { [0] = ROOT_TERM_COLOR_BLACK, [1] = ROOT_TERM_COLOR_RED,
        [2] = ROOT_TERM_COLOR_GREEN, [3] = ROOT_TERM_COLOR_BROWN,
        [4] = ROOT_TERM_COLOR_BLUE,  [5] = ROOT_TERM_COLOR_PURPLE,
        [6] = ROOT_TERM_COLOR_CYAN,  [7] = ROOT_TERM_COLOR_GRAY };

static unsigned char vt_bright_color_map[8]
    = { [0] = ROOT_TERM_COLOR_DARK_GRAY,   [1] = ROOT_TERM_COLOR_LIGHT_RED,
        [2] = ROOT_TERM_COLOR_LIGHT_GREEN, [3] = ROOT_TERM_COLOR_YELLOW,
        [4] = ROOT_TERM_COLOR_LIGHT_BLUE,  [5] = ROOT_TERM_COLOR_LIGHT_PURPLE,
        [6] = ROOT_TERM_COLOR_LIGHT_CYAN,  [7] = ROOT_TERM_COLOR_WHITE };

void
root_term_init (root_term_t *term)
{
  if (term == NULL)
    {
      root_seterrno (ROOT_EINVAL);
      return;
    }
  term->stdout.base.read = NULL;
  term->stdout.base.write = root_term_write;
  term->stdout.base.ioctl = root_term_ioctl;
  term->stdout.term = term;
  term->state = ROOT_TERM_STATE_WRI;
  root_memset (&term->args, 0, sizeof (root_term_args_t));
  term->saved_cursor_pos.x = 0;
  term->saved_cursor_pos.y = 0;
  term->next = NULL;
}

void
root_register_term (root_term_t *term)
{
  int first = terms == NULL;
  if (term == NULL)
    {
      root_seterrno (ROOT_EINVAL);
      return;
    }
  term->next = terms;
  terms = term;
  if (first)
    root_term_set_primary (term);
}

root_err_t
root_unregister_term (root_term_t *term)
{
  root_term_t *p = NULL, *t = terms;
  if (term == NULL)
    return ROOT_EINVAL;
  while (t != NULL)
    {
      if (t == term)
        {
          if (p == NULL)
            terms = term->next;
          else
            p->next = term->next;
          term->next = NULL;
          return ROOT_SUCCESS;
        }
      p = t;
      t = t->next;
    }
  return ROOT_ENOENT;
}

root_term_t *
root_iterate_terms (root_term_t *term)
{
  return term == NULL ? terms : term->next;
}

root_term_t *
root_term_get_primary (void)
{
  return primary_term;
}

void
root_term_set_primary (root_term_t *term)
{
  if (root_running_task == NULL || root_running_task->nfds <= ROOT_STDOUT)
    /* TODO: think about if primary term is NULL here */
    root_error ("term: unable to set primary term");
  primary_term = term;
  if (term != NULL)
    {
      root_running_task->fds[ROOT_STDOUT] = &term->stdout.base;
      term->clr (term);
    }
  else
    root_running_task->fds[ROOT_STDOUT] = NULL;
}

static void
term_handle_esc_seq (root_term_t *term, unsigned char code)
{
#define DEFAULT_ARG(X) term->args.buf[0].i == 0 ? (X) : term->args.buf[0].i
  root_term_pos_t xy = term->getxy (term);
  root_term_size_t wh = term->getwh (term);
  switch (code)
    {
    case ROOT_TERM_CSI_CUU:
      {
        int move = DEFAULT_ARG (1);
        move = MIN (xy.y, move);
        term->setxy (term, xy.x, xy.y - move);
        break;
      }
    case ROOT_TERM_CSI_CUD:
      {
        int move = DEFAULT_ARG (1);
        move = MIN (wh.height - xy.y - 1, move);
        term->setxy (term, xy.x, xy.y + move);
        break;
      }
    case ROOT_TERM_CSI_CUF:
      {
        int move = DEFAULT_ARG (1);
        move = MIN (wh.width - xy.x - 1, move);
        term->setxy (term, xy.x + move, xy.y);
        break;
      }
    case ROOT_TERM_CSI_CUB:
      {
        int move = DEFAULT_ARG (1);
        move = MIN (xy.x, move);
        term->setxy (term, xy.x - move, xy.y);
        break;
      }
    case ROOT_TERM_CSI_CNL:
      break;
    case ROOT_TERM_CSI_CPL:
      break;
    case ROOT_TERM_CSI_CHA:
      break;
    case ROOT_TERM_CSI_CUP:
      break;
    case ROOT_TERM_CSI_ED:
      break;
    case ROOT_TERM_CSI_EL:
      break;
    case ROOT_TERM_CSI_SU:
      break;
    case ROOT_TERM_CSI_SD:
      break;
    case ROOT_TERM_CSI_HVP:
      break;
    case ROOT_TERM_CSI_SGR:
      {
        for (root_size_t i = 0; i < term->args.cnt; i++)
          {
            int arg = term->args.buf[i].i;
            if (arg == 0)
              {
                term->setfg (term, TERM_DEFAULT_FG);
                term->setbg (term, TERM_DEFAULT_BG);
              }
            else if (arg >= 30 && arg <= 37)
              term->setfg (term, vt_color_map[arg - 30]);
            else if (arg == 39)
              term->setfg (term, TERM_DEFAULT_FG);
            else if (arg >= 40 && arg <= 47)
              term->setbg (term, vt_color_map[arg - 40]);
            else if (arg == 49)
              term->setbg (term, TERM_DEFAULT_BG);
            else if (arg >= 90 && arg <= 97)
              term->setfg (term, vt_bright_color_map[arg - 90]);
            else if (arg >= 100 && arg <= 107)
              term->setbg (term, vt_bright_color_map[arg - 100]);
          }
        break;
      }
    case ROOT_TERM_CSI_SCP:
      term->saved_cursor_pos = xy;
      break;
    case ROOT_TERM_CSI_RCP:
      term->setxy (term, term->saved_cursor_pos.x, term->saved_cursor_pos.y);
      break;
    }
  term->args.cnt = 0;
  term->state = ROOT_TERM_STATE_WRI;
#undef DEFAULT_ARG
}

void
root_term_putchar (root_term_t *term, char ch)
{
  root_term_size_t wh;
  root_term_pos_t xy;
  if (term == NULL)
    {
      root_seterrno (ROOT_EINVAL);
      return;
    }
  wh = term->getwh (term);
  xy = term->getxy (term);
  switch (term->state)
    {
    case ROOT_TERM_STATE_WRI:
      if (ch == '\033')
        {
          term->state = ROOT_TERM_STATE_ESC;
          return;
        }
      break;
    case ROOT_TERM_STATE_ESC:
      switch (ch)
        {
        case '[':
          term->state = ROOT_TERM_STATE_CSI;
          if (term->args.buf == NULL)
            term->args.buf = root_zalloc (ROOT_PAGE_SIZE);
          if (term->args.buf == NULL)
            {
              term->state = ROOT_TERM_STATE_WRI;
              return;
            }
          term->args.buf[0].i = 0;
          break;
        default:
          term->state = ROOT_TERM_STATE_WRI;
          break;
        }
      return;
    case ROOT_TERM_STATE_CSI:
      {
        int *i = &term->args.buf[term->args.cnt].i;
        if (ch >= '0' && ch <= '9')
          *i = *i * 10 + (ch - '0');
        else if (ch == ';')
          term->args.buf[++term->args.cnt].i = 0;
        else
          {
            ++term->args.cnt;
            term_handle_esc_seq (term, ch);
          }
        return;
      }
    }
  switch (ch)
    {
    case '\b':
      if (xy.x)
        term->setxy (term, xy.x - 1, xy.y);
      else if (xy.y)
        term->setxy (term, wh.width - 1, xy.y - 1);
      break;
    case '\n':
      term->setxy (term, 0, xy.y);
      // fallthrough
    case '\r':
      term->newline (term);
      break;
    default:
      term->putchar (term, ch);
      term->advance (term);
      break;
    }
}

void
root_term_sync_cursor (root_term_t *term)
{
  root_term_pos_t pos;
  if (term == NULL)
    {
      root_seterrno (ROOT_EINVAL);
      return;
    }
  pos = term->getxy (term);
  term->putcursor (term, pos.x, pos.y);
}

root_ssize_t
root_term_write (root_file_t *file, const char *buf, root_size_t size)
{
  root_term_file_t *term_file;
  if (file == NULL || buf == NULL)
    return ROOT_EINVAL;
  term_file = (root_term_file_t *) file;
  for (root_size_t i = 0; i < size; i++)
    root_term_putchar (term_file->term, buf[i]);
  return size;
}

int
root_term_ioctl (root_file_t *file, int op, va_list args)
{
  root_term_file_t *term_file;
  root_term_t *term;
  if (file == NULL)
    return ROOT_EINVAL;
  term_file = (root_term_file_t *) file;
  term = term_file->term;
  if (term == NULL)
    return ROOT_EINVAL;
  (void) args;
  switch (op)
    {
    case ROOT_TERM_IOCTL_SYNC_CURSOR:
      {
        root_term_pos_t xy = term->getxy (term);
        term->putcursor (term, xy.x, xy.y);
        break;
      }
    }
  return ROOT_EINVAL;
}
