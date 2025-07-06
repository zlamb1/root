#include "pci.h"
#include "i386/machine_io.h"
#include "memory/malloc.h"
#include "memory/page.h"
#include "string.h"
#include "types.h"

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA    0xCFC

static root_u16
root_pci_read_word (root_u8 bus, root_u8 dev, root_u8 func, root_u8 offset)
{
  root_u16 word;
  root_u32 address = offset & 0xFC;
  address |= (func & 0x7) << 8;
  address |= (dev & 0x1F) << 11;
  address |= bus << 16;
  address |= 0x80000000;
  root_outl (PCI_CONFIG_ADDRESS, address);
  word = (root_u16) ((root_inl (PCI_CONFIG_DATA) >> ((offset & 0x2) << 3))
                     & 0xFFFF);
  return word;
}

static root_u32
root_pci_read_long (root_u8 bus, root_u8 dev, root_u8 func, root_u8 offset)
{
  root_u32 _long;
  root_u32 address = offset & 0xFC;
  address |= (func & 0x7) << 8;
  address |= (dev & 0x1F) << 11;
  address |= bus << 16;
  address |= 0x80000000;
  root_outl (PCI_CONFIG_ADDRESS, address);
  _long = root_inl (PCI_CONFIG_DATA);
  return _long;
}

static root_pci_device_header_t
root_pci_read_hdr (root_u8 bus, root_u8 dev, root_u8 func)
{
  root_pci_device_header_t hdr = { .bus = bus, .dev = dev, .func = func };
  hdr.vendor_id = root_pci_read_word (bus, dev, func, 0);
  hdr.dev_id = root_pci_read_word (bus, dev, func, 2);
  hdr.prog_if = root_pci_read_word (bus, dev, func, 0x8) >> 8;
  hdr.subclass = root_pci_read_word (bus, dev, func, 0xA);
  hdr.class = root_pci_read_word (bus, dev, func, 0xA) >> 8;
  hdr.type = root_pci_read_word (bus, dev, func, 0xE);
  switch (hdr.type & ~0x80)
    {
    case 0:
      hdr.gdev.bar0 = root_pci_read_long (bus, dev, func, 0x10);
      hdr.gdev.bar1 = root_pci_read_long (bus, dev, func, 0x14);
      hdr.gdev.bar2 = root_pci_read_long (bus, dev, func, 0x18);
      hdr.gdev.bar3 = root_pci_read_long (bus, dev, func, 0x1C);
      hdr.gdev.bar4 = root_pci_read_long (bus, dev, func, 0x20);
      hdr.gdev.bar5 = root_pci_read_long (bus, dev, func, 0x24);
      hdr.gdev.irq_line = root_pci_read_word (bus, dev, func, 0x3C) & 0xFF;
      break;
    }
  return hdr;
}

static inline root_err_t
root_pci_device_append (root_pci_devices *devices,
                        root_pci_device_header_t hdr)
{
  if (devices->ndevices >= devices->cdevices)
    {
      root_pci_device_header_t *tmp;
      devices->cdevices += ROOT_PAGE_SIZE / sizeof (root_pci_device_header_t);
      tmp = root_realloc (devices->headers, sizeof (root_pci_device_header_t)
                                                * devices->cdevices);
      if (tmp == NULL)
        return ROOT_ERR_ALLOC;
      devices->headers = tmp;
    }
  devices->headers[devices->ndevices++] = hdr;
  return ROOT_SUCCESS;
}

root_err_t
root_pci_enumerate (root_pci_devices *devices)
{
  root_err_t err;
  if (devices == NULL)
    return ROOT_ERR_ARG;
  root_memset (devices, 0, sizeof (root_pci_devices));
  for (root_u16 bus = 0; bus < 256; bus++)
    {
      for (root_u8 device = 0; device < 32; device++)
        {
          root_pci_device_header_t header;
          if (root_pci_read_word (bus, device, 0, 0) == 0xFFFF)
            continue;
          header = root_pci_read_hdr (bus, device, 0);
          if ((err = root_pci_device_append (devices, header)) != ROOT_SUCCESS)
            return err;
          if (header.type & 0x80)
            {
              for (root_u8 func = 1; func < 8; func++)
                {
                  if (root_pci_read_word (bus, device, func, 0) == 0xFFFF)
                    continue;
                  header = root_pci_read_hdr (bus, device, func);
                  if ((err = root_pci_device_append (devices, header))
                      != ROOT_SUCCESS)
                    return err;
                }
            }
        }
    }
  return ROOT_SUCCESS;
}
