#ifndef ROOT_ALLOC_H
#define ROOT_ALLOC_H 1

int root_initalloc (void);
void *root_alloc_page (void);
void root_free_page (void *p);

#endif
