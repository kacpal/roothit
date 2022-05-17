# roothit
Anti-rootkit Loadable Kernel Module for Linux


## Introduction
This LKM was made for a school project with main objective to learn about defensive security. Most of the work is based off current and outdated linux rootkits and anti-rootkits, all of which will be linked down below. The rootkit was made and tested on Linux >5.7.X.


## Table of Contents
1. [Introduction](#introduction)
2. [Testing and Development](#testing-and-development)
3. [Configuration](#configuration)
4. [Usage](#usage)
5. [Methodology](#methodology)
      1. [Kprobe](#kprobe)
      2. [Modules](#modules)
      3. [Syscall](#syscall)
      4. [Memory](#memory) 
6. [Future possible mechanisms](#future-possible-mechanims)
7. [References](#references)


## Testing and Development
The module is being tested on [Ubuntu 20.04 LTS](https://ubuntu.com/download/desktop) with Linux 5.13.0. Compilation is done inside the testing environment. In order to compile, simply use `make`, then you can load the module with `sudo insmod roothit.ko` and unload with `sudo rmmod roothit.ko`. You can also test it using Makefile with `make test`, that loads compiled module, runs one iteration of all checks and displays dmesg output. Please note that in order to use it, `SCHEDULE_CHECKS` in config must be set to 0.

The roothit was tested against [Diamorphine](https://github.com/m0nad/Diamorphine) and [brokepkg](https://github.com/R3tr074/brokepkg) rootkits, mianly because they provide a selection of common rootkit techinques and are working on newer Linux versions.

To provide better quality of code styling we use help of the `clang-format`.


## Configuration
You can configure roothit using `config.h`. There you can select which checks do you want to disable or leave enabled. If `SCHEDULE_CHECKS` is set to 1, selected checks will be performed periodically. If set to 0, roothit will run just one iteration and exit.


## Usage
You can also communicate with the module while it's running through procfs (file `/proc/roothit`). Echo 0 to disable task scheduler, 1 to enable and 2 to once run all checks. 


## Methodology
### Kprobe
In order to use some of kernel functions, first we need to hook them. There are multiple way to do so, and we're using kprobe because it's effective, somewhat elegant and works for newer kernel versions. Kprobe is very mature Linux API designed for debugging and tracing but all we care for now is that after putting a kprobe on a function, we can simply get its pointer and use a function for ourselves:
``
register_kprobe(&kp)
func_ptr = (unsigned long)kp.addr;
``
We can use this method to fetch not only very important functions but also `sys_call_table`! This is very handy for some of our anti-rootkit checks. 

### Modules
Sysfs contains data structures called `kobject` (kernel object), each of which is contained in `kset` - collection of kobjects. Kobjects contain different members, one of which might be a `module` that, as you probably expect, are data structures for Linux loadable kernel modules. Each module is contained in a single linked list. 

Very common practice for many LKM rootkits is hiding by removing themselves from the list of modules. This can be acomplished by one simple line of code: `list_del(&THIS_MODULE->list);`. Modules can be referenced directly from the module list, but also from kset - and we can take advantage of that. We're iterating over the kset, and comparing each module extracted from `kobject` against standard module list. Whenever there is a module that is present in the kset but absent in module list - we can be pretty sure that the module is trying to hide itself from userspace.

Another addtitional approach is checking each result from kset if its list_head's were tinkered in any way. That is to see whether they point to `NULL`, `LIST_POISON` or to themselves. 

When any suspected module is detected, roothit tries to unhide it by simply linking it back to the module list. While this is not the cleanest way to do this, at least it might give possibility for user to examine.

### Syscall
Syscall hijacking is another very popular practice among linux rootkits. Syscalls (or system calls) are kernel functions that allow userspace to do many basic but crucial things, for example: `read`, `write` or `execve`.  Each syscall has its own handler function and ID that are mapped in `sys_call_table`. The basic idea for rootkit here is to create hook that refers to the real syscall function but before calling it, do _something more_.

One of the most basic protections against it is copying `sys_call_table` at initialization of the anti-rootkit module and later on, comparing real table against the stored copy. Of course this assumes that the anti-rootkit was loaded before any rootkit.

That's why the second check is much more interesting. We're checking each entry in syscall table to see whether it points outside of its dedicated memory section - which is inside core kernel text. We can do this by using `core_kernel_text` function from [Linux kernel](https://elixir.bootlin.com/linux/v5.13/source/kernel/extable.c#L73).

After hijacked syscall is detected, we can restore the original one from our saved copy. Because `sys_call_table` resides on readonly page, we will need to disable readonly memory protection. This can be done simply by flipping bit 16 of cr0 register (only for x86 systems). This techinque is also used by most rootkits.

### Memory
Another method to addtionally check security of loaded modules is to look for certain symbols in their memory. We can achieve that by simply iterating over sections of memory that are described by module kernel objects. `roothit` is looking for code that can disable cr0 memory protection and a short list of example strings. 


## Future possible mechanims
- Check for hidden open ports
- Check for hidden files


## References
- https://github.com/xcellerator/linux_kernel_hacking
- https://github.com/R3tr074/brokepkg
- https://github.com/m0nad/Diamorphine
- https://nbulischeck.github.io/tyton/
- https://github.com/linuxthor/rkspotter
- https://www.kernel.org/doc/html/latest/trace/kprobes.html
- https://linux-kernel-labs.github.io/refs/heads/master/labs/kernel_modules.html
- https://jm33.me/hook-system-calls-in-linux-5x.html
- https://foxtrot-sq.medium.com/linux-rootkits-multiple-ways-to-hook-syscall-s-7001cc02a1e6