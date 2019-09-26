#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "include/multiboot.h"
#include "graphics/DebugDisplay.h"
#include "cpu/hal.h"
#include "cpu/gdt.h"
#include "cpu/idt.h"
#include "cpu/pit.h"
#include "system/tss.h"
#include "system/sysapi.h"
#include "system/exception.h"
#include "system/user.h"
#include "system/time.h"
#include "system/task.h"
#include "devices/kybrd.h"
#include "devices/pci.h"
#include "memory/pmm.h"
#include "memory/vmm.h"
#include "devices/ata.h"
#include "fs/vfs.h"
#include "fs/ext2/ext2.h"

extern vfs_file_system_type ext2_fs_type;
extern thread *current_thread;

int kernel_main(uint32_t boot_magic, multiboot_info_t *boot_info)
{
  if (boot_magic != MULTIBOOT_BOOTLOADER_MAGIC)
  {
    DebugPrintf("\nBootloader MAGIC is invalid... Halting");
    return -1;
  }

  DebugClrScr(0x13);
  DebugGotoXY(0, 0);
  DebugSetColor(0x17);

  // gdt including kernel, user and tss
  gdt_init();
  install_tss(5, 0x10, 0);

  // register irq and handlers
  idt_init();
  exception_init();

  // timer and keyboard
  pit_init();
  kkybrd_install();

  // enable interrupts to start irqs (timer, keyboard)
  enable_interrupts();

  // physical memory and paging
  pmm_init(boot_info);
  vmm_init();

  // register system apis
  syscall_init();

  task_init(kernel_init);

  return 0;
}

void kernel_init()
{

  pci_scan_buses();

  vfs_init(&ext2_fs_type, "/dev/hda");

  // setup stack and enter user mode
  setup_and_enter_usermode();
}