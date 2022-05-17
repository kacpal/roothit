#include <linux/proc_fs.h>
#include "scheduler.h"

#define BUFFER_SIZE 1

static struct proc_dir_entry *proc_ent;

static ssize_t p_write(struct file *file, const char __user *ubuf, size_t count, loff_t *ppos) {
	return -1;
}

static ssize_t p_read(struct file *file, char __user *ubuf, size_t count, loff_t *ppos) {
	char buf[BUFFER_SIZE];

      if (copy_from_user(ubuf, buf, 1))
            return -EFAULT;

      if (buf[0] == 0) {
            sched_exit();
            printk(KERN_DEBUG "roothit: Scheduled tasks switched off\n");
      }
      if (buf[0] == 1) {
            sched_init();
            printk(KERN_DEBUG "roothit: Scheduled tasks switched on\n");
      }
      if (buf[0] == 2) {
            run_checks();
            printk(KERN_DEBUG "roothit: Running all checks\n");
      }

	return count;
}

struct proc_ops fops = {
	.proc_read = p_read,
	.proc_write = p_write,
};

void init_procfs(void) {
      proc_ent = proc_create("roothit", 0644, NULL, &fops);
}

void cleanup_procfs(void) {
      proc_remove(proc_ent);
}