#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/uio.h>  
#include <linux/namei.h>
#include <linux/mount.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/uaccess.h>
#include <linux/pagemap.h>
#include <linux/kprobes.h>
#include <linux/sched/task.h>
#include <linux/version.h>
#include <linux/extable.h>
#include "util.h"


int mem_search(const void *addr, int len1, const void *code, int len2) {
  while (len1 >= len2) {
      len1--;
      if (memcmp(addr, code, len2) == 0)
          return -1;
      addr++;
  }
  return 0;
}

int check_symbols(unsigned long *addr, int len) {
  char *cr0_rax[1] = {"diamorphine_"};

  if(mem_search(addr, len, (char *)cr0_rax[0], strlen((char *)cr0_rax[0])) != 0)
      return -1; 

  return 0; 
}

void run_memory_check(void) {
  unsigned long addr;

  printk("roothit: Running memory check\n");
  for (addr = MODULES_VADDR; addr < MODULES_END; addr += 4096) {

    // TODO how to check if addr is readable?
    if(kernel_write(NULL, &addr, 0, 0) == -1)
      continue;

    printk("roothit: Checking %lx\n", addr);
    if(check_symbols(&addr, 4096) != 0) {
          printk(KERN_ALERT "roothit: %lx conatins suspicious code\n", addr);
    }
  }
}

void init_memory_check(void) {}