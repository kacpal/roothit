#include <linux/kallsyms.h>
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/slab.h>

#include "memory.h"
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
    }

    if (module->list.next == NULL || module->list.prev == NULL) {
      printk(KERN_ALERT "Module %s is hidden\n", module->name);
    }

    // search in standard module list
    if (!find_mod(mod_kobj->mod)) {
      printk(KERN_ALERT "Module %s is hidden\n", module->name);
    }

    // memory checks
    if (check_symbols(
            (module->core_layout.base + module->core_layout.text_size),
            (module->core_layout.ro_size - module->core_layout.text_size)) !=
        0) {
      printk(KERN_ALERT "Module %s contains suspicious string\n", module->name);
    }

    if (check_code(module->core_layout.base, module->core_layout.text_size) !=
        0) {
      printk(KERN_ALERT "Module %s contains suspicious code\n", module->name);
    }
  }
}

void init_module_check(void) { run_module_check(); }