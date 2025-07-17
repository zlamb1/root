#include "term.h"
#include "errno.h"
#include "machine.h"
#include "print.h"
#include "task.h"

static root_term_t *terms = NULL;

void
root_term_init (root_term_t *term)
{
  if (term == NULL)
    {
      root_seterrno (ROOT_EARG);
      return;
    }
  term->stdout.base.read = NULL;
  term->stdout.base.write = root_term_write;
  term->stdout.base.ioctl = root_term_ioctl;
  term->stdout.term = term;
}

void
root_register_term (root_term_t *term)
{
  int first = terms == NULL;
  if (term == NULL)
    {
      root_seterrno (ROOT_EARG);
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
    return ROOT_EARG;
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

void
root_term_set_primary (root_term_t *term)
{
  if (root_running_task == NULL || root_running_task->nfds <= ROOT_STDOUT)
    root_halt ();
  if (term != NULL)
    {
      term->init (term);
      root_running_task->fds[ROOT_STDOUT] = &term->stdout.base;
    }
  else
    root_running_task->fds[ROOT_STDOUT] = NULL;
}

void
root_term_putchar (root_term_t *term, char ch)
{
  root_term_size_t wh;
  root_term_pos_t xy;
  if (term == NULL)
    {
      root_seterrno (ROOT_EARG);
      return;
    }
  wh = term->getwh (term);
  xy = term->getxy (term);
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

root_ssize_t
root_term_write (root_file_t *file, const char *buf, root_size_t size)
{
  root_term_file_t *term_file;
  if (file == NULL || buf == NULL)
    return ROOT_EARG;
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
    return ROOT_EARG;
  term_file = (root_term_file_t *) file;
  term = term_file->term;
  if (term == NULL)
    return ROOT_EARG;
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
  return ROOT_EUOP;
}
