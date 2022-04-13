#ifndef UTIL_H
#define UTIL_H

#include <linux/kallsyms.h>

extern unsigned long fetch_function(char *f_name);

void protect_mem(void);

void unprotect_mem(void);

void init_util(void);

#endif /* UTIL_H */