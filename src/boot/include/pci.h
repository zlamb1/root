#ifndef ROOT_PCI_H
#define ROOT_PCI_H 1

#include "types.h"

typedef struct
{
  root_uint32_t bar0;
  root_uint32_t bar1;
  root_uint32_t bar2;
  root_uint32_t bar3;
  root_uint32_t bar4;
  root_uint32_t bar5;
  root_uint8_t irq_line;
} root_pci_gen_header_t;

typedef struct
{
  root_uint8_t bus;
  root_uint8_t dev;
  root_uint8_t func;

  root_uint16_t vendor_id;
  root_uint16_t dev_id;
  root_uint8_t prog_if;
  root_uint8_t subclass;
  root_uint8_t class;
  root_uint8_t type;

  union
  {
    root_pci_gen_header_t ghdr;
  };
} root_pci_header_t;

typedef struct
{
  root_uint32_t nheaders, cheaders;
  root_pci_header_t *headers;
} root_pci_headers_t;

typedef struct root_pci_devmod_t
{
  root_uint8_t cls, scls;
  root_err_t (*init) (root_pci_header_t *hdr);
  struct root_pci_devmod_t *next;
} root_pci_devmod_t;

root_err_t root_pci_register_devmod (root_uint8_t cls, root_uint8_t scls,
                                     int (*init) (root_pci_header_t *hdr));

root_err_t root_pci_register_devmods (void);

root_err_t root_pci_enumerate (root_pci_headers_t *headers);

#endif
