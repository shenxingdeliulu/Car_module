KERNELDIR = /opt/FriendlyARM/linux-2.6.32.2
    # The current directory is passed to sub-makes as argument
PWD := $(shell pwd)

CROSS_COMPILE	=arm-linux-
CC	= $(CROSS_COMPILE)gcc

obj-m :=18B20.o 

modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions

.PHONY: modules modules_install clean



