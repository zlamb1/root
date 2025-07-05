#include "memory/page.h"
#include "types.h"

root_page_t *
root_get_page (void *p)
{
  root_uintptr_t addr = (root_uintptr_t) p;
  root_uintptr_t index = addr >> ROOT_PAGE_POW;
  if (index >= root_page_map.num_pages)
    return NULL;
  return root_page_map.pages + index;
}
