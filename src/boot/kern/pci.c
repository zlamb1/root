#include "pci.h"
#include "disk/ata.h"
#include "i386/machine_io.h"
#include "memory/malloc.h"
#include "memory/page.h"
#include "string.h"
#include "types.h"

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA    0xCFC

static root_pci_devmod_t *root = NULL;

static root_uint16_t
root_pci_read_word (root_uint8_t bus, root_uint8_t dev, root_uint8_t func,
                    root_uint8_t offset)
{
  root_uint16_t word;
  root_uint32_t address = offset & 0xFC;
  address |= (func & 0x7) << 8;
  address |= (dev & 0x1F) << 11;
  address |= bus << 16;
  address |= 0x80000000;
  root_outl (PCI_CONFIG_ADDRESS, address);
  word = (root_uint16_t) ((root_inl (PCI_CONFIG_DATA) >> ((offset & 0x2) << 3))
                          & 0xFFFF);
  return word;
}

static root_uint32_t
root_pci_read_long (root_uint8_t bus, root_uint8_t dev, root_uint8_t func,
                    root_uint8_t offset)
{
  root_uint32_t _long;
  root_uint32_t address = offset & 0xFC;
  address |= (func & 0x7) << 8;
  address |= (dev & 0x1F) << 11;
  address |= bus << 16;
  address |= 0x80000000;
  root_outl (PCI_CONFIG_ADDRESS, address);
  _long = root_inl (PCI_CONFIG_DATA);
  return _long;
}

static root_pci_header_t
root_pci_read_header (root_uint8_t bus, root_uint8_t dev, root_uint8_t func)
{
  root_pci_header_t header = { .bus = bus, .dev = dev, .func = func };
  header.vendor_id = root_pci_read_word (bus, dev, func, 0);
  header.dev_id = root_pci_read_word (bus, dev, func, 2);
  header.prog_if = root_pci_read_word (bus, dev, func, 0x8) >> 8;
  header.subclass = root_pci_read_word (bus, dev, func, 0xA);
  header.class = root_pci_read_word (bus, dev, func, 0xA) >> 8;
  header.type = root_pci_read_word (bus, dev, func, 0xE);
  switch (header.type & ~0x80)
    {
    case 0:
      header.ghdr.bar0 = root_pci_read_long (bus, dev, func, 0x10);
      header.ghdr.bar1 = root_pci_read_long (bus, dev, func, 0x14);
      header.ghdr.bar2 = root_pci_read_long (bus, dev, func, 0x18);
      header.ghdr.bar3 = root_pci_read_long (bus, dev, func, 0x1C);
      header.ghdr.bar4 = root_pci_read_long (bus, dev, func, 0x20);
      header.ghdr.bar5 = root_pci_read_long (bus, dev, func, 0x24);
      header.ghdr.irq_line = root_pci_read_word (bus, dev, func, 0x3C) & 0xFF;
      break;
    }
  return header;
}

static inline root_err_t
root_pci_device_append (root_pci_headers_t *headers, root_pci_header_t header)
{
  if (headers->nheaders >= headers->cheaders)
    {
      root_pci_header_t *tmp;
      headers->cheaders += ROOT_PAGE_SIZE / sizeof (root_pci_header_t);
      tmp = root_realloc (headers->headers,
                          sizeof (root_pci_header_t) * headers->cheaders);
      if (tmp == NULL)
        return ROOT_EALLOC;
      headers->headers = tmp;
    }
  headers->headers[headers->nheaders++] = header;
  return ROOT_SUCCESS;
}

static root_pci_devmod_t *
root_pci_find_mod (root_uint8_t cls, root_uint8_t scls)
{
  /* TODO: think about hashmap instead */
  root_pci_devmod_t *devmod = root;
  while (devmod != NULL)
    {
      if (devmod->cls == cls && devmod->scls == scls)
        return devmod;
      devmod = devmod->next;
    }
  return NULL;
}

root_err_t
root_pci_register_devmod (root_uint8_t cls, root_uint8_t scls,
                          int (*init) (root_pci_header_t *hdr))
{
  root_pci_devmod_t *devmod = root_pci_find_mod (cls, scls);
  if (devmod == NULL)
    {
      devmod = root_malloc (sizeof (root_pci_devmod_t));
      if (devmod == NULL)
        return ROOT_EALLOC;
      devmod->cls = cls;
      devmod->scls = scls;
      devmod->init = init;
      devmod->next = root;
      root = devmod;
    }
  else
    devmod->init = init;
  return ROOT_SUCCESS;
}

root_err_t
root_pci_register_devmods (void)
{
  root_err_t err;
  if ((err = root_pci_register_devmod (1, 1, root_ata_init_controller))
      != ROOT_SUCCESS)
    return err;
  return ROOT_SUCCESS;
}

root_err_t
root_pci_enumerate (root_pci_headers_t *headers)
{
  root_err_t err;
  if (headers == NULL)
    return ROOT_EARG;
  root_memset (headers, 0, sizeof (root_pci_headers_t));
  for (root_uint16_t bus = 0; bus < 256; bus++)
    {
      for (root_uint8_t device = 0; device < 32; device++)
        {
          root_pci_header_t header;
          if (root_pci_read_word (bus, device, 0, 0) == 0xFFFF)
            continue;
          header = root_pci_read_header (bus, device, 0);
          if ((err = root_pci_device_append (headers, header)) != ROOT_SUCCESS)
            return err;
          if (header.type & 0x80)
            {
              for (root_uint8_t func = 1; func < 8; func++)
                {
                  if (root_pci_read_word (bus, device, func, 0) == 0xFFFF)
                    continue;
                  header = root_pci_read_header (bus, device, func);
                  if ((err = root_pci_device_append (headers, header))
                      != ROOT_SUCCESS)
                    return err;
                }
            }
        }
    }
  for (root_size_t i = 0; i < headers->nheaders; i++)
    {
      root_pci_header_t *header = headers->headers + i;
      root_pci_devmod_t *devmod
          = root_pci_find_mod (header->class, header->subclass);
      if (devmod != NULL)
        devmod->init (header);
    }
  return ROOT_SUCCESS;
}
