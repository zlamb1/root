#include "i386-pc/bios.h"
#include "kern/disk.h"
#include "kern/malloc.h"
#include "kern/mod.h"
#include "kern/print.h"
#include "kern/string.h"
#include "kern/types.h"

typedef struct
{
  root_uint16_t buf_size;
  root_uint16_t flags;
  root_uint32_t cylinders;
  root_uint32_t heads;
  root_uint32_t sectors_per_track;
  root_uint64_t total_sectors;
  root_uint16_t bytes_per_sector;
  root_uint32_t edd;
  root_uint16_t device_path_sig;
  root_uint8_t device_path_len;
  root_uint8_t reserved0[3];
  char host_bus[4];
  char interface_type[8];
  char interface_path[8];
  char device_path[8];
  char reserved1;
  unsigned char checksum;
} __attribute__ ((packed)) bios_drive_parameters_t;

typedef struct
{
  root_uint8_t packet_size;
  root_uint8_t reserved0;
  root_uint16_t num_blocks;
  root_uint16_t buffer_offset;
  root_uint16_t buffer_segment;
  root_uint64_t lba;
  root_uint64_t buffer_lba;
} __attribute__ ((packed)) disk_read_packet_t;

typedef struct root_biosdisk_t
{
  root_disk_t disk;
  unsigned char drive_number;
  struct root_biosdisk_t *next;
} root_biosdisk_t;

static root_biosdisk_t *disks;

extern unsigned char BOOT_DRIVE_NUMBER;

static root_ssize_t
biosdisk_read (root_disk_t *disk, char *buf, root_size_t sec, root_size_t nsec)
{
  root_biosdisk_t *biosdisk;
  root_size_t read = 0;
  /* NOTE: it is very important that we use a buffer below 1MiB for the BIOS
   * int */
  char tmpbuf[ROOT_SECTOR_SIZE];
  if (disk == NULL || (buf == NULL && nsec))
    return ROOT_EINVAL;
  biosdisk = (root_biosdisk_t *) disk;
  while (sec + read < nsec && sec < disk->nsec)
    {
      root_bios_args_t args = { 0 };
      disk_read_packet_t packet = { 0 };
      args.eax = 0x42 << 8;
      args.edx = biosdisk->drive_number;
      args.ds = root_get_real_segment (&packet);
      args.esi = root_get_real_offset (&packet);
      packet.packet_size = sizeof (disk_read_packet_t);
      packet.num_blocks = 1;
      packet.lba = sec + read;
      packet.buffer_segment = root_get_real_segment (tmpbuf);
      packet.buffer_offset = root_get_real_offset (tmpbuf);
      root_bios_interrupt (0x13, &args);
      if (args.flags & ROOT_BIOS_CARRY_FLAG)
        return ROOT_EIO;
      root_memcpy (buf, tmpbuf, ROOT_SECTOR_SIZE);
      buf += ROOT_SECTOR_SIZE;
      read++;
    }
  return read;
}

ROOT_MOD_INIT (biosdisk)
{
  root_biosdisk_t *biosdisk;
  root_bios_args_t args = { 0 };
  bios_drive_parameters_t params = { 0 };
  unsigned char bda_total_disks = *(unsigned char *) (0x475);
  unsigned char total_disks = 0;
  for (unsigned drive = 0x80; drive <= 0xFF && total_disks < bda_total_disks;
       drive++)
    {
      args.eax = 0x41 << 8;
      args.ebx = 0x55AA;
      args.edx = (unsigned char) drive;
      root_bios_interrupt (0x13, &args);
      if (args.flags & ROOT_BIOS_CARRY_FLAG || (args.ebx & 0xFFFF) != 0xAA55
          || !(args.ecx & 1))
        {
          if (drive == BOOT_DRIVE_NUMBER)
            root_warn ("biosdisk: extended BIOS functions not supported "
                       "for boot drive");
          continue;
        }
      args.eax = 0x48 << 8;
      args.edx = BOOT_DRIVE_NUMBER;
      args.ds = root_get_real_segment (&params);
      args.esi = root_get_real_offset (&params);
      root_memset (&params, 0, sizeof (bios_drive_parameters_t));
      params.buf_size = sizeof (bios_drive_parameters_t);
      root_bios_interrupt (0x13, &args);
      if (args.flags & ROOT_BIOS_CARRY_FLAG)
        {
          root_warn ("biosdisk: failed to get drive parameters");
          continue;
        }
      biosdisk = root_malloc (sizeof (root_biosdisk_t));
      if (biosdisk == NULL)
        {
          root_warn ("biosdisk: out of memory");
          return;
        }
      biosdisk->drive_number = BOOT_DRIVE_NUMBER;
      biosdisk->disk.nsec = params.total_sectors;
      biosdisk->disk.disk_read = biosdisk_read;
      root_init_disk_dev (&biosdisk->disk);
      biosdisk->next = biosdisk;
      disks = biosdisk;
      root_disk_register (&biosdisk->disk);
      total_disks++;
    }
}

ROOT_MOD_FINI (biosdisk)
{
  root_biosdisk_t *biosdisk = disks;
  while (biosdisk != NULL)
    {
      root_biosdisk_t *n = biosdisk->next;
      root_disk_unregister (&biosdisk->disk);
      root_free_disk_dev (&biosdisk->disk);
      root_free (biosdisk);
      biosdisk = n;
    }
}
