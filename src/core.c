#include "core.h"
#include "config.h"
#include "modules.h"
#include "procfs.h"
#include "scheduler.h"
#include "syscall.h"
#include "util.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kacper Paluch");
MODULE_DESCRIPTION("Linux rootkit detection");
MODULE_VERSION("0.01");

static int __init lkm_example_init(void) {
  printk(KERN_INFO "roothit: starting\n");

  init_procfs();

#if SCHEDULE_CHECKS
  sched_init();
#else
  run_checks();
#endif /* SCHEDULE_CHECKS */

  return 0;
}

static void __exit lkm_example_exit(void) {
  sched_exit();
  cleanup_procfs();
  printk(KERN_INFO "roothit: exited\n");
}

module_init(lkm_example_init);
module_exit(lkm_example_exit);