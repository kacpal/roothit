#include "util.h"
#include <linux/extable.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/version.h>

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
  int i;
  char *symbol_list[8] = {"m0nad", "diamorphine", "rootkit", "syscall_table",
                          "cr0",   "hook",        "brokepkg"};

  for (i = 0; i < sizeof(*symbol_list); i++) {
    if (mem_search(addr, len, (char *)symbol_list[0],
                   strlen((char *)symbol_list[0])) != 0)
      return -1;
  }

  return 0;
}

int check_code(unsigned long *addr, int len) {
  char cr0[3] = {'\x0f', '\x22', '\xc0'};

  if (mem_search(addr, len, cr0, 3) != 0)
    return -1;
  return 0;
}