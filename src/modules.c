#include <linux/kallsyms.h>
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/slab.h>

#include "util.h"

static struct list_head *real_module_list;

bool find_mod(struct module *mod) {
  bool on_list = false;
  struct module *cur, *tmp;

  real_module_list = THIS_MODULE->list.prev;

  list_for_each_entry_safe(cur, tmp, real_module_list, list) {
    if (cur == mod)
      on_list = true;
  }

  return on_list;
}

// TODO find more stable way to unhide modules
void unhide_mod(struct module *mod) {
  list_add(&mod->list, THIS_MODULE->list.next);

  if (mod->exit != NULL)
    mod->exit();
}

// int _memsrch(const void *addr, int len1, const void *code, int len2)
// {
//   while (len1 >= len2) 
//   {
//       len1--;
//       if (memcmp(addr, code, len2) == 0)
//       {
//           return -1;
//       }
//       addr++;
//   }
//   return 0;
// }

// int lkm_code_check(unsigned long *addr, int len)
// {
//   // code signatures.. 
//   //
//   // 0f 22 c0            mov    %rax,%cr0
//   char cr0_rax[3] = {'\x0f','\x22','\xc0'};

//   if(_memsrch(addr, len, cr0_rax, 3) != 0)
//   {
//       return -1; 
//   }
//   return 0; 
// }

void run_module_check(void) {
  struct kobject *cur, *tmp;
  struct module *module;
  struct module_kobject *mod_kobj;
  struct kset *module_kset;

  module_kset = THIS_MODULE->mkobj.kobj.kset;

  list_for_each_entry_safe(cur, tmp, &module_kset->list, entry) {
    mod_kobj = container_of(tmp, struct module_kobject, kobj);

    // check if kobject is a module
    if (!mod_kobj || !mod_kobj->mod)
      continue;

    module = mod_kobj->mod;

    // ignore ourselves
    if (module == THIS_MODULE)
      continue;

    // check for module list tampering
    if (module->list.next == module->list.prev) {
      printk(KERN_ALERT "Module %s is hidden\n", module->name);
      // unhide_mod(module);
    }

    if (module->list.next == NULL || module->list.prev == NULL) {
      printk(KERN_ALERT "Module %s is hidden\n", module->name);
      // unhide_mod(module);
    }

    if (module->list.next == LIST_POISON1 ||
        module->list.prev == LIST_POISON2) {
      printk(KERN_ALERT "Module %s is hidden\n", module->name);
      // unhide_mod(module);
    }

    // search in standard module list
    if (!find_mod(mod_kobj->mod)) {
      printk(KERN_ALERT "Module %s is hidden\n", module->name);
      // unhide_mod(module);
    }

    // // test
    // if (lkm_code_check(mod_kobj->mod->core_layout.base, mod_kobj->mod->core_layout.text_size) != 0) {
    //   printk(KERN_ALERT "Module %s contains sus code\n", module->name);
    //   // unhide_mod(module);
    // }
  }
}

void init_module_check(void) { run_module_check(); }