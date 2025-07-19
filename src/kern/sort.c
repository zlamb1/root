#include "kern/sort.h"
#include "kern/string.h"

void
root_ssort (void *arr, root_size_t nelements, root_size_t elsize,
            int (*cmp) (const void *, const void *))
{
  char *carr = (char *) arr;
  char tmp[elsize];
  if (nelements == 0)
    return;
  for (root_size_t i = 0; i < nelements - 1; i++)
    {
      root_size_t min_index = i;
      for (root_size_t j = i + 1; j < nelements; j++)
        {
          const void *cur_el = carr + j * elsize;
          const void *min_el = carr + min_index * elsize;
          if (cmp (cur_el, min_el) < 0)
            min_index = j;
        }
      if (min_index != i)
        {
          char *cur_el = carr + i * elsize;
          char *min_el = carr + min_index * elsize;
          root_memcpy (tmp, cur_el, elsize);
          root_memcpy (cur_el, min_el, elsize);
          root_memcpy (min_el, tmp, elsize);
        }
    }
}
