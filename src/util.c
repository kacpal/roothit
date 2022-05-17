#include "util.h"
#include "core.h"
#include "syscall.h"

#include <linux/kprobes.h>

static struct kprobe kp;

unsigned long fetch_function(char *f_name) {
  unsigned long ptr;

  kp.symbol_name = f_name;

  if (register_kprobe(&kp) < 0)
    return 0;

  ptr = (unsigned long)kp.addr;
  unregister_kprobe(&kp);

  return ptr;
}

static inline void write_cr0_forced(unsigned long val) {
  unsigned long __force_order;

  asm volatile("mov %0, %%cr0" : "+r"(val), "+m"(__force_order));
}

void protect_mem(void) { write_cr0_forced(read_cr0()); }

void unprotect_mem(void) { write_cr0_forced(read_cr0() & ~0x00010000); }