#ifndef ROOT_ATA_H
#define ROOT_ATA_H

#include "pci.h"
#include "types.h"

typedef enum
{
  ATA_BUS_PRIMARY = 0x0,
  ATA_BUS_SECONDARY = 0x1
} root_ata_bus_t;

typedef enum
{
  ROOT_ATA_DRIVE_PRIMARY = 0x0,
  ROOT_ATA_DRIVE_SECONDARY = 0x1
} root_ata_drive_t;

typedef enum
{
  ROOT_ATA_MODE_COMPAT,
  ROOT_ATA_MODE_PCI
} root_ata_mode_t;

typedef struct root_disk_t
{
  root_size_t nsectors;
  root_ssize_t (*read) (struct root_disk_t *dev, root_size_t sector, char *buf,
                        size_t nsectors);
  struct root_disk_t *next;
} root_disk_t;

typedef struct
{
  root_ata_mode_t mode;
  root_u16 io_ports[2];
  root_u16 ctrl_ports[2];
  root_ata_drive_t selected[2];
  root_u8 bus_mastering;
  root_u8 refcount;
} root_ata_controller_t;

typedef struct
{
  root_disk_t disk;
  root_ata_controller_t *controller;
  root_ata_bus_t bus;
  root_ata_drive_t drive;
} root_ata_disk_t;

root_err_t root_init_ata_controller (root_pci_device_header_t *hdr);

#endif
