#include <linux/proc_fs.h>
#include "scheduler.h"

#define BUFFER_SIZE 1

static struct proc_dir_entry *proc_ent;

static ssize_t p_read(struct file *file, char __user *ubuf, size_t count, loff_t *ppos) {
	return -1;
}

static ssize_t p_write(struct file *file, const char __user *ubuf, size_t count, loff_t *ppos) {
	char buf[BUFFER_SIZE];
      int i;

      if (copy_from_user(buf, ubuf, 1))
            return -EFAULT;

	sscanf(buf, "%d", &i);
      if (i == 0) {
            sched_exit();
            printk(KERN_INFO "roothit: Scheduled tasks switched off\n");
      }
      if (i == 1) {
            sched_init();
            printk(KERN_INFO "roothit: Scheduled tasks switched on\n");
      }
      if (i == 2) {
            run_checks();
            printk(KERN_INFO "roothit: Running all checks\n");
      }

	return count;
}

struct proc_ops fops = {
	.proc_read = p_read,
	.proc_write = p_write,
};

void init_procfs(void) {
      proc_ent = proc_create("roothit", 0666, NULL, &fops);
}

void cleanup_procfs(void) {
      proc_remove(proc_ent);
}