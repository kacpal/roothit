#include "syscall.h"
#include "core.h"
#include "scheduler.h"
#include "util.h"

unsigned long *sct = NULL; // sys_call_table
static unsigned long sct_copy[NR_syscalls];

int (*ckt)(unsigned long addr) = NULL; // core_kernel_text

static struct kprobe kp = {
    .symbol_name = "kallsyms_lookup_name",
};

static void retrive_sct(int num) {
  unprotect_mem();
  sct[num] = sct_copy[num];
  protect_mem();
}

void check_sct(void) {
  char f_name_buff[128];
  unsigned long addr;
  int i;

  printk(KERN_INFO "roothit: Checking sys_call_table");

  for (i = 0; i < NR_syscalls; i++) {
    addr = sct[i];

    if (!ckt(addr)) {
      sprint_symbol(f_name_buff, sct[i]);
      printk(KERN_ALERT "%s (%lx) is outside of kernel space", f_name_buff,
             addr);
      retrive_sct(i);
    }

    if (sct_copy[i] != sct[i]) {
      sprint_symbol(f_name_buff, sct[i]);
      printk(KERN_ALERT "%s (%lx) has been altered\n", f_name_buff, addr);
      retrive_sct(i);
    }
  }
}

void fetch_sct(void) {
  typedef unsigned long (*kallsyms_lookup_name_t)(const char *name);
  kallsyms_lookup_name_t kln;

  register_kprobe(&kp);
  kln = (kallsyms_lookup_name_t)kp.addr;
  unregister_kprobe(&kp);

  sct = (unsigned long *)kln("sys_call_table");
}

void init_sct_check(void) {
  int i;

  printk(KERN_INFO "roothit: Initalizing sys_call_table checks");
  fetch_sct();
  ckt = (void *)fetch_function("core_kernel_text");

  for (i = 0; i < NR_syscalls; i++) {
    sct_copy[i] = sct[i];
  }
}

void run_syscall_check(void) {
  if (!sct) {
    init_sct_check();
  }
  check_sct();
}