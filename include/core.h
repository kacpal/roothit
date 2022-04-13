#ifndef CORE_H
#define CORE_H

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>

// typedef void (* f_ptr)(int param);
// typedef unsigned long (*f_ptr)(void);
typedef unsigned long (*f_ptr)(const char *name);


#endif /* CORE_H */