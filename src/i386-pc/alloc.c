#include "kern/alloc.h"
#include "common/page.h"
#include "kern/errno.h"
#include "kern/print.h"
#include "kern/string.h"
#include "kern/types.h"

#define MMAP_START 0x100000
#define MMAP_END   0xFFFFFFFF

root_page_map_t root_page_map;

static root_size_t first_upage = 0, last_upage = 0, hint_upage = 0;

static root_uintptr_t
get_prev_page_address (root_uintptr_t page_address)
{
  if (page_address == first_upage)
    return last_upage;
  return page_address - ROOT_PAGE_SIZE;
}

int
root_init_alloc (root_mmap_t *mmap)
{
  root_mmap_entry_t *entry, *pm_entry;
  root_size_t top_entry, num_pages;
  root_uint64_t page_map_sz = 0;
  int first_page = 1;
  root_page_map.pages = NULL;
  root_page_map.num_pages = 0;
  root_page_map.num_upages = 0;
  for (root_size_t i = mmap->nentries; i > 0; i--)
    {
      root_uint64_t sz;
      entry = mmap->entries + (i - 1);
      if (entry->base > MMAP_END)
        continue;
      if (entry->base + entry->len > MMAP_END)
        sz = MMAP_END - entry->base;
      else
        sz = entry->len;
      num_pages = ROOT_PAGE_ALIGN_UP (entry->base + sz) >> ROOT_PAGE_POW;
      page_map_sz = sizeof (root_page_t) * num_pages;
      top_entry = i;
      break;
    }
  if (!page_map_sz)
    return ROOT_EALLOC;
  for (root_size_t i = top_entry; i > 0; i--)
    {
      root_uint64_t len, end, aligned_end;
      entry = mmap->entries + (i - 1);
      if (entry->type != ROOT_MMAP_ENTRY_TYPE_FREE || entry->len < page_map_sz)
        continue;
      len = entry->len;
      end = entry->base + len;
      if (entry->base < MMAP_START)
        {
          if (end > MMAP_START)
            {
              len = end - MMAP_START;
              if (len < page_map_sz)
                continue;
            }
          else
            continue;
        }
      aligned_end = ROOT_PAGE_ALIGN_DOWN (end);
      if (len - (end - aligned_end) < page_map_sz)
        continue;
      root_page_map.pages
          = (root_page_t *) (root_uintptr_t) (aligned_end - page_map_sz);
      root_memset (root_page_map.pages, 0, page_map_sz);
      pm_entry = entry;
      break;
    }
  if (root_page_map.pages == NULL)
    return ROOT_EALLOC;
  root_page_map.num_pages = num_pages;
  for (root_size_t i = 0; i < top_entry; i++)
    {
      root_uint64_t aligned_base, end;
      entry = mmap->entries + i;
      if (entry->type != ROOT_MMAP_ENTRY_TYPE_FREE || entry->base < MMAP_START
          || entry->len < ROOT_PAGE_SIZE)
        continue;
      if (entry->base < MMAP_START)
        {
          if (entry->base + entry->len > MMAP_START)
            aligned_base = MMAP_START;
          else
            continue;
        }
      else
        aligned_base = ROOT_PAGE_ALIGN_UP (entry->base);
      if (entry == pm_entry)
        end = (root_uint64_t) (root_uintptr_t) root_page_map.pages;
      else
        end = entry->base + entry->len;
      if (first_page && aligned_base + ROOT_PAGE_SIZE <= end)
        {
          first_upage = aligned_base;
          first_page = 0;
        }
      while (aligned_base + ROOT_PAGE_SIZE <= end)
        {
          root_page_t *page
              = root_get_page ((void *) (uintptr_t) aligned_base);
          page->flags |= ROOT_PG_FLAG_FREE;
          root_page_map.num_upages++;
          last_upage = (root_size_t) aligned_base;
          aligned_base += ROOT_PAGE_SIZE;
        }
    }
  hint_upage = last_upage;
  return ROOT_SUCCESS;
}

void *
root_alloc_page (void)
{
  root_uintptr_t page_address = hint_upage;
  do
    {
      root_page_t *page = root_get_page ((void *) page_address);
      if (page->flags & ROOT_PG_FLAG_FREE)
        {
          page->flags &= ~ROOT_PG_FLAG_FREE & ~ROOT_PG_FLAG_TAIL;
          page->flags |= ROOT_PG_FLAG_HEAD;
          page->npages = 1;
          hint_upage = get_prev_page_address (page_address);
          return (void *) page_address;
        }
      page_address = get_prev_page_address (page_address);
    }
  while (page_address != hint_upage);
  return NULL;
}

static void *
root_try_alloc_pages (root_size_t npages, root_uintptr_t page_address,
                      root_size_t end_cond)
{
  root_page_t *page = root_get_page ((void *) page_address);
  for (; page_address >= end_cond; page_address -= ROOT_PAGE_SIZE, page--)
    {
      if (page->flags & ROOT_PG_FLAG_FREE)
        {
          root_size_t cnt = 1;
          while (cnt < npages)
            {
              page_address -= ROOT_PAGE_SIZE;
              page--;
              if (page->flags & ROOT_PG_FLAG_FREE)
                cnt++;
              else
                break;
            }
          if (cnt == npages)
            {
              root_uintptr_t start_address = page_address;
              void *p = (void *) page_address;
              page->flags &= (~ROOT_PG_FLAG_FREE & ~ROOT_PG_FLAG_TAIL);
              page->flags |= ROOT_PG_FLAG_HEAD;
              page->npages = npages;
              for (root_size_t i = 1; i < cnt; i++)
                {
                  page++;
                  page->flags &= (~ROOT_PG_FLAG_FREE & ~ROOT_PG_FLAG_HEAD);
                  page->flags |= ROOT_PG_FLAG_TAIL;
                }
              hint_upage = get_prev_page_address (start_address);
              return p;
            }
        }
    }
  return NULL;
}

void *
root_alloc_pages (root_uint32_t npages)
{
  root_uintptr_t page_address = hint_upage;
  void *p = root_try_alloc_pages (
      npages, page_address, first_upage + ((npages - 1) << ROOT_PAGE_POW));
  if (p != NULL)
    return p;
  page_address = last_upage;
  p = root_try_alloc_pages (npages, page_address, hint_upage);
  if (p == NULL)
    root_seterrno (ROOT_EALLOC);
  return p;
}

void
root_free_pages (const void *p)
{
  root_uintptr_t page_address = (root_uintptr_t) p;
  root_page_t *page;
  root_uint32_t npages;
  if (p == NULL)
    root_error ("alloc: attempted to free null pointer");
  if (page_address % ROOT_PAGE_SIZE)
    root_error ("alloc: attempted to free misaligned page");
  page = root_get_page (p);
  if (page->flags & ROOT_PG_FLAG_FREE)
    root_error ("alloc: double free detected");
  if ((page->flags & ROOT_PG_FLAG_HEAD) == 0)
    root_error ("alloc: attempted to free non-head page");
  npages = page->npages;
  while (npages--)
    {
      page = root_get_page ((void *) page_address);
      page->flags &= ~ROOT_PG_FLAG_HEAD & ~ROOT_PG_FLAG_TAIL;
      page->flags |= ROOT_PG_FLAG_FREE;
      page_address += ROOT_PAGE_SIZE;
    }
}
