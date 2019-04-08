#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "include/multiboot.h"
#include "graphics/DebugDisplay.h"
#include "memory/pmm.h"

char *strMemoryTypes[] = {

    "Available",
    "Reserved",
    "ACPI Reclaim",
    "ACPI NVS Memory",
    "Bad Memory"};

int kernel_main(uint32_t boot_magic, multiboot_info_t *boot_info)
{

  if (boot_magic != MULTIBOOT_BOOTLOADER_MAGIC)
  {
    DebugPrintf("Bootloader MAGIC is invalid... Halting\n");
    return -1;
  }

  DebugClrScr(0x13);
  DebugGotoXY(0, 0);
  DebugSetColor(0x17);

  DebugPrintf("\nmem_lower = %dKB, mem_upper = %dKB",
              (unsigned)boot_info->mem_lower, (unsigned)boot_info->mem_upper);
  DebugPrintf("\nkernel start = 0x%x end = 0x%x", KERNEL_START, KERNEL_END);
  DebugPrintf("\nmem map: 0x%x %d", (unsigned)boot_info->mmap_addr, (unsigned)boot_info->mmap_length);

  pmm_init(boot_info);

  multiboot_memory_map_t *region = (multiboot_memory_map_t *)boot_info->mmap_addr;

  for (int i = 0; region < boot_info->mmap_addr + boot_info->mmap_length; i++)
  {
    if (region->type > 4 && region->addr == 0)
      break;

    DebugPrintf("\nregion %i: start = 0x%x, length (bytes): 0x%x, type = %d (%s)", i,
                (unsigned)region->addr,
                (unsigned)region->len,
                (unsigned)region->type, strMemoryTypes[(unsigned)region->type - 1]);

    pmm_init_region(region->addr, region->len);

    region = (multiboot_memory_map_t *)((unsigned long)region + region->size + sizeof(region->size));
  }
  pmm_deinit_region(0x100000, KERNEL_END - KERNEL_START);

  return 0;
}