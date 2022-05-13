NAME := roothit
SRCDIR := src

obj-m := $(NAME).o
$(NAME)-y += $(SRCDIR)/core.o
$(NAME)-y += $(SRCDIR)/util.o
$(NAME)-y += $(SRCDIR)/modules.o
$(NAME)-y += $(SRCDIR)/syscall.o
$(NAME)-y += $(SRCDIR)/scheduler.o
$(NAME)-y += $(SRCDIR)/memory.o


HEADERS := $(PWD)/include
ccflags-y += -I$(HEADERS)

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

test:
	sudo dmesg -C
	sudo insmod $(NAME).ko
	sudo rmmod $(NAME).ko
	dmesg
