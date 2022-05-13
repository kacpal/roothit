#include "scheduler.h"
#include "config.h"
#include "core.h"
#include "modules.h"
#include "syscall.h"
#include "util.h"
#include "memory.h"

static struct work_struct work;
static void work_handler(struct work_struct *data);

DECLARE_DELAYED_WORK(delayed_work, work_handler);

void run_checks(void) {
#if CHECK_MODULE_LIST
  run_module_check();
#endif /* CHECK_MODULE_LIST */

#if CHECK_SYSCALL_TABLE
  run_syscall_check();
#endif /* CHECK SYSCALL_TABLE */

#if CHECK_MEMORY
  run_memory_check();
#endif /* CHECK_MEMORY */
}

static void work_handler(struct work_struct *data) {
  run_checks();
  schedule_delayed_work(&delayed_work,
                        __msecs_to_jiffies(SCHEDULER_INTERVAL_MS));
}

int sched_init(void) {
  INIT_WORK(&work, work_handler);
  init_util();
  
  run_checks();
  schedule_delayed_work(&delayed_work,
                        __msecs_to_jiffies(SCHEDULER_INTERVAL_MS));

  return 0;
}

int sched_exit(void) {
  flush_delayed_work(&delayed_work);
  cancel_delayed_work(&delayed_work);

  if (!delayed_work_pending(&delayed_work))
    return -1;
  return 0;
}