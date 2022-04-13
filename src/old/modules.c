#include <linux/list.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/kallsyms.h>

static LIST_HEAD(modules);
static struct list_head *real_module_list;


bool check(struct module *mod) {
  bool on_list = false;
  struct module *cur, *tmp;

  real_module_list = THIS_MODULE->list.prev;

  list_for_each_entry_safe(cur, tmp, real_module_list, list) {
    if (cur == mod)
      on_list = true;
    }

  return on_list;
}

// void analyze_mem(struct module_kobject *a) {
//     struct kobject *b;
//     struct module_kobject *kobj;

//     printk(KERN_ALERT "Check 1: %s | %p\n", a->mod->name, a);
    
//     b = (struct kobject*)(a->mod->core_layout.base + a->mod->core_layout.size);
//     kobj = container_of(b, struct module_kobject, kobj);
    
//     printk(KERN_ALERT "Check 2: \n[b]: %p\n[base] %lx\n[size] %lx\n", b, (uintptr_t)a->mod->core_layout.base, (uintptr_t)a->mod->core_layout.size);

//     // printk(KERN_ALERT "Check 2: %s | %p\n", kobj->mod->name, kobj);
// }

void analyze(void) {
    struct kobject *cur, *tmp;
    struct module_kobject *kobj; 

    struct kset *module_kset;

    module_kset = THIS_MODULE->mkobj.kobj.kset;

    list_for_each_entry_safe(cur, tmp, &module_kset->list, entry) {        
        kobj = container_of(tmp, struct module_kobject, kobj);
        
        if (!kobj || !kobj->mod)
			    continue;

        if (!check(kobj->mod))
          printk(KERN_ALERT "Module %s is hidden\n", kobj->mod->name);
    }
}

void mem_check(void) {
  struct module *mod;
  unsigned long addr;
  struct module_kobject *kobj; 

  char buff[100];

  for (addr = MODULES_VADDR; addr < MODULES_END; addr += 1024) {
    if(is_module_text_address(addr))
      printk(KERN_ALERT "lol");
  }
}