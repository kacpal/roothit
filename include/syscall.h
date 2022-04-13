#ifndef SYSCALL_H
#define SYSCALL_H

#include <asm/asm-offsets.h> // for NR_syscalls
#include <linux/slab.h>
#include <linux/kprobes.h>
#include <linux/kallsyms.h>
#include <linux/string.h>

void fetch_sys_call_table(void);

void init_syscall_check(void);

void cmp_syscall_table(void);

void run_syscall_check(void);

#endif /* SYSCALL_H */