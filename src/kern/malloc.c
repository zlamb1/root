#include "kern/malloc.h"
#include "common/page.h"
#include "kern/alloc.h"
#include "kern/print.h"
#include "kern/slab.h"
#include "kern/string.h"

#define MAX_SLABS 9

root_slab_t *slabs[MAX_SLABS];

static inline root_size_t
root_get_pow2_sz (root_size_t sz)
{
  if (sz < 8)
    sz = 8;
  /* align up to power of two */
  sz--;
  sz |= sz >> 1;
  sz |= sz >> 2;
  sz |= sz >> 4;
  sz |= sz >> 8;
  sz |= sz >> 16;
  sz++;
  return sz;
}

static inline root_size_t
root_get_slab_idx (root_size_t sz)
{
  root_size_t index = 0;
  root_size_t b = sz >> 3;
  while (!(b & 0x1))
    {
      b >>= 1;
      index++;
    }
  return index;
}

void *
root_malloc (root_size_t sz)
{
  if (sz > 2048)
    {
      root_size_t npages = ROOT_PAGE_ALIGN_UP (sz) >> ROOT_PAGE_POW;
      return root_alloc_pages (npages);
    }
  else
    {
      root_slab_t *slab;
      root_size_t index;
      void *p;
      sz = root_get_pow2_sz (sz);
      index = root_get_slab_idx (sz);
      if (index >= MAX_SLABS)
        root_error ("malloc: invalid index");
      slab = slabs[index];
      if (slab == NULL)
        {
          slab = root_create_slab (sz, sz);
          if (slab == NULL)
            return NULL;
          slab->flags |= ROOT_SLAB_FLAG_MALLOC;
          slabs[index] = slab;
          return root_slab_alloc (slab);
        }
      if (!slab->cnt)
        root_error ("malloc: exhausted slab");
      p = root_slab_alloc (slab);
      if (p == NULL)
        return NULL;
      if (!slab->cnt)
        {
          if (slab->prev == NULL)
            slabs[index] = slab->next;
          else
            slab->prev->next = slab->next;
          if (slab->next != NULL)
            slab->next->prev = slab->prev;
          slab->prev = NULL;
          slab->next = NULL;
        }
      return p;
    }
}

void *
root_realloc (const void *p, root_size_t sz)
{
  root_page_t *page;
  void *np;
  if (p == NULL)
    return root_malloc (sz);
  page = root_get_page (p);
  if (page == NULL)
    root_error ("realloc: invalid page");
  if (page->flags & ROOT_PG_FLAG_FREE)
    root_error ("realloc: double free detected");
  np = root_malloc (sz);
  if (np == NULL)
    return NULL;
  if (page->flags & ROOT_PG_FLAG_SLAB)
    {
      root_slab_t *slab = page->slab;
      if (slab == NULL)
        root_error ("realloc: invalid slab");
      root_memcpy (np, p, sz > slab->sz ? slab->sz : sz);
    }
  else
    {
      root_size_t oldsz = page->npages << ROOT_PAGE_POW;
      root_memcpy (np, p, sz > oldsz ? oldsz : sz);
    }
  root_free (p);
  return np;
}

void
root_free (const void *p)
{
  root_page_t *page = root_get_page (p);
  if (page == NULL)
    root_error ("free: invalid page");
  if (page->flags & ROOT_PG_FLAG_FREE)
    root_error ("free: double free detected");
  if (page->flags & ROOT_PG_FLAG_SLAB)
    {
      root_slab_t *slab = page->slab, *iter;
      root_size_t index;
      int found = 0;
      if (slab == NULL)
        root_error ("free: invalid slab");
      if (!(slab->flags & ROOT_SLAB_FLAG_MALLOC))
        root_error ("free: non-malloc slab");
      index = root_get_slab_idx (slab->sz);
      if (index >= MAX_SLABS)
        root_error ("free: invalid index");
      iter = slabs[index];
      while (iter != NULL)
        {
          if (iter == slab)
            {
              found = 1;
              break;
            }
          iter = iter->next;
        }
      if (found)
        {
          root_slab_t *prev = slab->prev;
          root_slab_t *next = slab->next;
          if (root_slab_free (slab, p, 1))
            {
              if (prev == NULL)
                slabs[index] = next;
              else
                prev->next = next;
              if (next != NULL)
                next->prev = prev;
            }
        }
      else if (!root_slab_free (slab, p, 1))
        {
          /* freed object and slab isn't released */
          slab->prev = NULL;
          slab->next = slabs[index];
          if (slab->next != NULL)
            slab->next->prev = slab;
          slabs[index] = slab;
        }
    }
  else
    root_free_pages (p);
}
