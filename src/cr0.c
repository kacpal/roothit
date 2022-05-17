#include "util.h"
#include <linux/kprobes.h>

void run_cr0_check(void) {
  if ((read_cr0() & 0x00010000) == 0) {
    printk(KERN_ALERT "roothit: Write protection disabled\n");
  }
}