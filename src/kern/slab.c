#include "kern/slab.h"
#include "common/page.h"
#include "kern/alloc.h"
#include "kern/print.h"

#define SLAB_TARGET_SIZE 8
#define ALIGN(X, A)      (((X) + ((A) - 1)) & ~((A) - 1))

static root_slab_t *slab_slab = NULL;

static void
init_slab_pages (root_slab_t *slab, void *pages, root_size_t npages)
{
  root_page_t *page;
  root_uintptr_t page_address = (root_uintptr_t) pages;
  while (npages--)
    {
      page = root_get_page ((void *) page_address);
      if (page == NULL)
        root_error ("slab: null page");
      page->flags |= ROOT_PG_FLAG_SLAB;
      page->slab = slab;
      page_address += ROOT_PAGE_SIZE;
    }
}

static void
init_slab_freelist (root_size_t cap, root_node_t *freelist, root_size_t sz)
{
  root_node_t *node;
  root_uintptr_t address = (root_uintptr_t) freelist;
  for (root_size_t i = 1; i < cap; i++)
    {
      address += sz;
      node = (root_node_t *) address;
      freelist->next = node;
      freelist = node;
    }
  freelist->next = NULL;
}

static root_slab_t *
root_init_slab_slab (void)
{
  root_slab_t *slab;
  root_size_t total_sz
      = ROOT_PAGE_ALIGN_UP (sizeof (root_slab_t) * SLAB_TARGET_SIZE);
  root_size_t npages = total_sz >> ROOT_PAGE_POW;
  root_size_t cap = (total_sz / sizeof (root_slab_t)) - 1;
  void *pages = root_alloc_pages (npages);
  if (pages == NULL)
    return NULL;
  slab = pages;
  slab->sz = sizeof (root_slab_t);
  slab->cnt = cap;
  slab->cap = cap;
  slab->pages = pages;
  slab->freelist
      = (root_node_t *) ((root_uintptr_t) pages + sizeof (root_slab_t));
  slab->prev = NULL;
  slab->next = NULL;
  slab->flags |= ROOT_SLAB_FLAG_SLAB_SLAB;
  init_slab_pages (slab, pages, npages);
  init_slab_freelist (cap, slab->freelist, sizeof (root_slab_t));
  return slab;
}

root_slab_t *
root_create_slab (root_size_t sz, root_size_t align)
{
  root_slab_t *slab;
  root_size_t aligned_sz;
  root_size_t total_sz;
  root_size_t npages;
  root_size_t cap;
  void *pages;
  if (slab_slab == NULL)
    {
      slab = root_init_slab_slab ();
      if (slab == NULL)
        return NULL;
      slab_slab = slab;
    }
  slab = root_slab_alloc (slab_slab);
  if (slab == NULL)
    return NULL;
  aligned_sz = ALIGN (sz, align);
  total_sz = ROOT_PAGE_ALIGN_UP (aligned_sz * SLAB_TARGET_SIZE);
  npages = total_sz >> ROOT_PAGE_POW;
  cap = total_sz / aligned_sz;
  pages = root_alloc_pages (npages);
  if (pages == NULL)
    {
      root_slab_free (slab_slab, slab, 0);
      return NULL;
    }
  slab->sz = aligned_sz;
  slab->cnt = cap;
  slab->cap = cap;
  slab->pages = pages;
  slab->freelist = (root_node_t *) pages;
  slab->prev = NULL;
  slab->next = NULL;
  init_slab_pages (slab, pages, npages);
  init_slab_freelist (cap, slab->freelist, aligned_sz);
  return slab;
}

void *
root_slab_alloc (root_slab_t *slab)
{
  root_node_t *node;
  if (slab == NULL)
    root_error ("slab_alloc: null slab");
  if (!slab->cnt)
    return NULL;
  node = slab->freelist;
  slab->freelist = node->next;
  slab->cnt--;
  if (!slab->cnt && slab->flags & ROOT_SLAB_FLAG_SLAB_SLAB)
    {
      if (slab->prev == NULL)
        slab_slab = slab->next;
      else
        slab->prev->next = slab->next;
      if (slab->next != NULL)
        slab->next->prev = slab->prev;
      slab->prev = NULL;
      slab->next = NULL;
    }
  return (void *) node;
}

int
root_slab_free (root_slab_t *slab, void *p, int release)
{
  int was_empty;
  root_node_t *node = (root_node_t *) p;
  if (slab == NULL)
    root_error ("slab_free: null slab");
  if (p == NULL)
    root_error ("slab_free: null pointer");
  if (slab->cnt == slab->cap)
    root_error ("slab_free: full slab");
  was_empty = !slab->cnt;
  node->next = slab->freelist;
  slab->freelist = node;
  slab->cnt++;
  if (was_empty && slab->flags & ROOT_SLAB_FLAG_SLAB_SLAB)
    {
      slab->prev = NULL;
      slab->next = slab_slab;
      if (slab_slab != NULL)
        slab_slab->prev = slab;
      slab_slab = slab;
    }
  if (release && slab->cnt == slab->cap)
    {
      root_slab_release (slab);
      return 1;
    }
  return 0;
}

void
root_slab_release (root_slab_t *slab)
{
  root_page_t *page;
  root_uintptr_t page_address;
  if (slab == NULL)
    root_error ("slab_release: null slab");
  if (slab->flags & ROOT_SLAB_FLAG_SLAB_SLAB)
    {
      if (slab->prev == NULL)
        slab_slab = slab->next;
      else
        slab->prev->next = slab->next;
      if (slab->next != NULL)
        slab->next->prev = slab->prev;
      /* only free pages; slab slabs do not have a parent */
      root_free_pages (slab->pages);
      return;
    }
  root_free_pages (slab->pages);
  slab->pages = NULL;
  page_address = ROOT_PAGE_ALIGN_DOWN ((root_uintptr_t) slab);
  page = root_get_page ((void *) page_address);
  if (page == NULL)
    root_error ("slab_release: null page");
  if (!(page->flags & ROOT_PG_FLAG_SLAB))
    root_error ("slab_release: invalid slab flag");
  root_slab_free (page->slab, (void *) slab, 1);
}
