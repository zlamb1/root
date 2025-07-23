#include "kern/disk.h"
#include "kern/mod.h"

typedef struct root_biosdisk_t
{
  root_disk_t base;
  struct root_biosdisk_t *next;
} root_biosdisk_t;

static root_biosdisk_t disks;

ROOT_MOD_INIT (biosdisk) {}

ROOT_MOD_FINI (biosdisk) {}
