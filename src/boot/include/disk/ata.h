#ifndef ROOT_ATA_H
#define ROOT_ATA_H

#include "disk/disk.h"
#include "pci.h"
#include "types.h"

typedef enum
{
  ROOT_ATA_BUS_PRIMARY = 0x0,
  ROOT_ATA_BUS_SECONDARY = 0x1
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

typedef struct
{
  root_ata_mode_t mode;
  root_uint16_t io_ports[2];
  root_uint16_t ctrl_ports[2];
  root_ata_drive_t selected[2];
  root_uint8_t bus_mastering;
  root_uint8_t refcount;
} root_ata_controller_t;

typedef struct
{
  root_disk_t base;
  root_ata_controller_t *controller;
  root_ata_bus_t bus;
  root_ata_drive_t drive;
} root_ata_disk_t;

root_err_t root_ata_init_controller (root_pci_header_t *header);

#endif
