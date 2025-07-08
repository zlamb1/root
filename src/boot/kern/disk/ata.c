#include "disk/ata.h"
#include "disk/disk.h"
#include "i386/machine_io.h"
#include "memory/malloc.h"
#include "panic.h"
#include "types.h"

#define ATA_SUCCESS   0
#define ATA_ENODEV    -1
#define ATA_EATAPIDEV -2
#define ATA_EDEVERR   -3

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

#define ATA_CMD_READ     0x20
#define ATA_CMD_IDENTIFY 0xEC

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

static int
root_ata_identify (root_ata_controller_t *dev, root_ata_bus_t bus,
                   root_ata_drive_t drive, void *buf)
{
  root_u8 status;
  root_u16 *wbuf = (root_u16 *) buf;
  root_ata_select_drive (dev, bus, 1, drive);
  root_ata_io_wait (dev, bus);
  root_outb (dev->io_ports[bus] + ATA_IO_SECTOR_CNT_REG, 0);
  root_outb (dev->io_ports[bus] + ATA_IO_LBA_LO_REG, 0);
  root_outb (dev->io_ports[bus] + ATA_IO_LBA_MID_REG, 0);
  root_outb (dev->io_ports[bus] + ATA_IO_LBA_HI_REG, 0);
  root_outb (dev->io_ports[bus] + ATA_IO_CMD_REG, ATA_CMD_IDENTIFY);
  status = root_inb (dev->io_ports[bus] + ATA_IO_STATUS_REG);
  if (!status)
    return ATA_ENODEV;
  do
    status = root_inb (dev->io_ports[bus] + ATA_IO_STATUS_REG);
  while (status & ATA_STATUS_BSY);
  if (root_inb (dev->io_ports[bus] + ATA_IO_LBA_MID_REG)
      || root_inb (dev->io_ports[bus] + ATA_IO_LBA_HI_REG))
    return ATA_EATAPIDEV;
  do
    status = root_inb (dev->io_ports[bus] + ATA_IO_STATUS_REG);
  while (!(status & ATA_STATUS_ERR) && !(status & ATA_STATUS_DRQ));
  if (status & ATA_STATUS_ERR)
    return ATA_EDEVERR;
  for (int i = 0; i < 256; i++)
    *wbuf++ = root_inw (dev->io_ports[bus] + ATA_IO_DATA_REG);
  return ATA_SUCCESS;
}

static root_err_t
root_ata_controller_read (root_ata_controller_t *dev, root_ata_bus_t bus,
                          root_ata_drive_t drive, root_size_t sector,
                          void *buf, root_size_t nsectors)
{
  root_u16 *wbuf;
  if (dev == NULL || buf == NULL || !nsectors)
    return ROOT_ERR_ARG;
  bus &= 1;
  drive &= 1;
  // TODO: handle oob reads
  if (dev->selected[bus] != drive)
    {
      root_ata_select_drive (dev, bus, 1, drive);
      root_ata_io_wait (dev, bus);
    }
  wbuf = (root_u16 *) buf;
  while (nsectors)
    {
      root_size_t read = nsectors > 256 ? 256 : nsectors;
      root_outb (dev->io_ports[bus] + ATA_IO_SECTOR_CNT_REG,
                 read == 256 ? 0 : read);
      root_outb (dev->io_ports[bus] + ATA_IO_LBA_LO_REG, sector);
      root_outb (dev->io_ports[bus] + ATA_IO_LBA_MID_REG, sector >> 8);
      root_outb (dev->io_ports[bus] + ATA_IO_LBA_HI_REG, sector >> 16);
      // TODO: set last 4 bits of 28-bit LBA
      root_outb (dev->io_ports[bus] + ATA_IO_CMD_REG, ATA_CMD_READ);
      nsectors -= read;
      while (read--)
        {
          root_err_t err = root_ata_poll (dev, bus);
          if (err != ROOT_SUCCESS)
            return ROOT_EDEV;
          for (int i = 0; i < 256; i++)
            *wbuf++ = root_inw (dev->io_ports[bus] + ATA_IO_DATA_REG);
          root_ata_io_wait (dev, bus);
        }
    }
  return ROOT_SUCCESS;
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
  uint64_t buf[64];
  int *lbuf = (int *) buf;
  short *wbuf = (short *) buf;

  root_ata_controller_t *controller;
  if (header == NULL || header->class != 1 || header->subclass != 1)
    return ROOT_ERR_ARG;

  controller = root_malloc (sizeof (root_ata_controller_t));
  if (controller == NULL)
    return ROOT_ERR_ALLOC;

  // TODO: use PCI bars if in PCI native mode
  controller->io_ports[0] = ATA_PRIMARY_IO_PORT;
  controller->io_ports[1] = ATA_SECONDARY_IO_PORT;

  controller->ctrl_ports[0] = ATA_PRIMARY_CTRL_PORT;
  controller->ctrl_ports[1] = ATA_SECONDARY_CTRL_PORT;

  for (int bus = 0; bus <= 1; bus++)
    {
      for (int drive = 0; drive <= 1; drive++)
        switch (root_ata_identify (controller, bus, drive, buf))
          {
          case ATA_SUCCESS:
            {
              root_err_t err;
              root_ata_disk_t *disk = root_malloc (sizeof (root_ata_disk_t));
              if (disk == NULL)
                {
                  root_free (controller);
                  return ROOT_ERR_ALLOC;
                }
              disk->controller = controller;
              disk->bus = bus;
              disk->drive = drive;
              if (buf[25])
                disk->base.nsectors = buf[25];
              else if (lbuf[30])
                disk->base.nsectors = lbuf[30];
              else
                root_panic ("ata: unknown nsectors");
              err = root_disk_register (&disk->base);
              if (err != ROOT_SUCCESS)
                {
                  root_free (disk);
                  root_free (controller);
                  return err;
                }
              controller->refcount++;
              break;
            }
          case ATA_ENODEV:
            break;
          default:
            return ROOT_ERR_ARG;
          }
    }

  if (!controller->refcount)
    {
      /* no available disks */
      root_free (controller);
      return ROOT_SUCCESS;
    }

  controller->selected[0] = 0;
  controller->selected[1] = 0;

  root_ata_select_drive (controller, ROOT_ATA_BUS_PRIMARY, 1,
                         controller->selected[0]);
  root_ata_select_drive (controller, ROOT_ATA_BUS_SECONDARY, 1,
                         controller->selected[1]);

  return ROOT_SUCCESS;
}
