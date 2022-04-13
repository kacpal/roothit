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

void unhide_mod(struct module *mod) {
  list_add(&mod->list, THIS_MODULE->list.next);

  if (mod->exit != NULL)
    mod->exit();
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

    // check for module list tampering
    if (module->list.next == module->list.prev) {
      printk(KERN_ALERT "Module %s is hidden\n", module->name);
      unhide_mod(module);
    }

    if (module->list.next == NULL || module->list.prev == NULL) {
      printk(KERN_ALERT "Module %s is hidden\n", module->name);
      unhide_mod(module);
    }

    if (module->list.next == LIST_POISON1 ||
        module->list.prev == LIST_POISON2) {
      printk(KERN_ALERT "Module %s is hidden\n", module->name);
      unhide_mod(module);
    }

    // search in standard module list
    if (!find_mod(mod_kobj->mod)) {
      printk(KERN_ALERT "Module %s is hidden\n", module->name);
      unhide_mod(module);
    }
  }
}

void init_module_check(void) { run_module_check(); }