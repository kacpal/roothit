#ifndef MEMORY_H
#define MEMORY_H

void run_memory_check(void);
int check_symbols(unsigned long *addr, int len);
int check_code(unsigned long *addr, int len);
void init_memory_check(void);

#endif /* MEMORY_H */