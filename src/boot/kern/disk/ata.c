#include "disk/ata.h"
#include "i386/machine_io.h"
#include "types.h"

#define ATA_PRIMARY_IO_PORT     0x1F0
#define ATA_SECONDARY_IO_PORT   0x170
#define ATA_PRIMARY_CTRL_PORT   0x3F6
#define ATA_SECONDARY_CTRL_PORT 0x376

#define ATA_IO_DATA_REG       0x0
#define ATA_IO_ERROR_REG      0x1
#define ATA_IO_FEATURES_REG   0x1
#define ATA_IO_SECTOR_CNT_REG 0x2
#define ATA_IO_LBA_LO_REG     0x3
#define ATA_IO_LBA_MID_REG    0x4
#define ATA_IO_LBA_HI_REG     0x5
#define ATA_IO_DRV_SEL_REG    0x6
#define ATA_IO_STATUS_REG     0x7
#define ATA_IO_CMD_REG        0x7

#define ATA_CTRL_ALT_STATUS_REG 0x0
#define ATA_CTRL_DEV_REG        0x0
#define ATA_CTRL_DRIVE_ADDR_REG 0x1

#define ATA_STATUS_ERR 0x01
#define ATA_STATUS_DRQ 0x08
#define ATA_STATUS_SRV 0x10
#define ATA_STATUS_DFE 0x20 /* drive fault error */
#define ATA_STATUS_RDY 0x40
#define ATA_STATUS_BSY 0x80

#define ATA_CMD_READ 0x20

#define ATA_PRIMARY_IRQ   14
#define ATA_SECONDARY_IRQ 15

static void
root_ata_io_wait (root_ata_controller_t *dev, root_ata_bus_t bus)
{
  /* wait ~400ns */
  root_u16 port = dev->ctrl_ports[bus & 0x1] + ATA_CTRL_ALT_STATUS_REG;
  for (int i = 0; i < 15; i++)
    root_inb (port);
}

static int
root_ata_poll (root_ata_controller_t *dev, root_ata_bus_t bus)
{
  root_u16 port = dev->ctrl_ports[bus & 0x1] + ATA_CTRL_ALT_STATUS_REG;
  root_u8 alt_status;
  do
    alt_status = root_inb (port);
  while (
      alt_status & ATA_STATUS_BSY
      || !(alt_status & (ATA_STATUS_ERR | ATA_STATUS_DRQ | ATA_STATUS_DFE)));
  if (alt_status & ATA_STATUS_ERR)
    return -1;
  if (alt_status & ATA_STATUS_DFE)
    return -2;
  return 0;
}

static void
root_ata_select_drive (root_ata_controller_t *dev, root_ata_bus_t bus,
                       root_u8 use_lba, root_ata_drive_t drive)
{
  root_u16 port = dev->io_ports[bus & 0x1] + ATA_IO_DRV_SEL_REG;
  root_u8 val = 0xA0;
  val |= (drive & 0x1) << 4;
  val |= (use_lba & 0x1) << 6;
  root_outb (port, val);
}

static void
root_ata_set_sector_cnt (root_ata_controller_t *dev, root_ata_bus_t bus,
                         root_u16 nsectors)
{
  root_u16 port = dev->io_ports[bus & 0x1] + ATA_IO_SECTOR_CNT_REG;
  /* TODO: support 48-bit LBA addressing */
  root_outb (port, nsectors);
}

static void
root_ata_set_lba24 (root_ata_controller_t *dev, root_ata_bus_t bus,
                    root_u32 lba24)
{
  root_u16 io_port = dev->io_ports[bus & 0x1];
  root_outb (io_port + ATA_IO_LBA_LO_REG, lba24);
  root_outb (io_port + ATA_IO_LBA_MID_REG, lba24 >> 8);
  root_outb (io_port + ATA_IO_LBA_HI_REG, lba24 >> 16);
}

static void
root_ata_send_command (root_ata_controller_t *dev, root_ata_bus_t bus,
                       root_u8 cmd)
{
  root_outb (dev->io_ports[bus & 0x1] + ATA_IO_CMD_REG, cmd);
}

root_err_t
root_ata_init_controller (root_pci_header_t *header)
{
  root_ata_controller_t controller;
  if (header == NULL || header->class != 1 || header->subclass != 1)
    return ROOT_ERR_ARG;
  // TODO: use PCI bars if in PCI native mode
  controller.io_ports[0] = ATA_PRIMARY_IO_PORT;
  controller.io_ports[1] = ATA_SECONDARY_IO_PORT;
  controller.ctrl_ports[0] = ATA_PRIMARY_CTRL_PORT;
  controller.ctrl_ports[1] = ATA_SECONDARY_CTRL_PORT;
  controller.selected[0] = 0;
  controller.selected[1] = 0;
  root_ata_select_drive (&controller, ATA_BUS_PRIMARY, 1,
                         controller.selected[0]);
  root_ata_select_drive (&controller, ATA_BUS_SECONDARY, 1,
                         controller.selected[1]);
  return ROOT_SUCCESS;
}
