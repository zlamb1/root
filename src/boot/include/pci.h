#ifndef ROOT_PCI_H
#define ROOT_PCI_H 1

#include "types.h"

typedef struct
{
  root_u32 bar0;
  root_u32 bar1;
  root_u32 bar2;
  root_u32 bar3;
  root_u32 bar4;
  root_u32 bar5;
  root_u8 irq_line;
} root_pci_gen_header_t;

typedef struct
{
  root_u8 bus;
  root_u8 dev;
  root_u8 func;

  root_u16 vendor_id;
  root_u16 dev_id;
  root_u8 prog_if;
  root_u8 subclass;
  root_u8 class;
  root_u8 type;

  union
  {
    root_pci_gen_header_t ghdr;
  };
} root_pci_header_t;

typedef struct
{
  root_u32 nheaders, cheaders;
  root_pci_header_t *headers;
} root_pci_headers_t;

typedef struct root_pci_devmod_t
{
  root_u8 cls, scls;
  root_err_t (*init) (root_pci_header_t *hdr);
  struct root_pci_devmod_t *next;
} root_pci_devmod_t;

root_err_t root_pci_register_devmod (root_u8 cls, root_u8 scls,
                                     int (*init) (root_pci_header_t *hdr));

root_err_t root_pci_register_devmods (void);

root_err_t root_pci_enumerate (root_pci_headers_t *headers);

#endif
