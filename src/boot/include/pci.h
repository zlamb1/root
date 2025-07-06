#ifndef ROOT_PCI_H

#include "types.h"

typedef struct root_gen_dev_t
{
  root_u32 bar0;
  root_u32 bar1;
  root_u32 bar2;
  root_u32 bar3;
  root_u32 bar4;
  root_u32 bar5;
  root_u8 irq_line;
} root_gen_dev_t;

typedef struct root_pci_device_header_t
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
    root_gen_dev_t gdev;
  };
} root_pci_device_header_t;

typedef struct root_pci_devices
{
  root_u32 ndevices, cdevices;
  root_pci_device_header_t *headers;
} root_pci_devices;

root_err_t root_pci_enumerate (root_pci_devices *devices);

#endif
