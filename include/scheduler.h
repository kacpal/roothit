#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <linux/workqueue.h>
#include <linux/sched.h>

int sched_init(void);
void run_checks(void);
int sched_exit(void);

#endif /* SCHEDULER_H */