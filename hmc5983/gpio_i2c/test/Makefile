#KERNELDIR = /opt/FriendlyARM/linux-3.0.8
    # The current directory is passed to sub-makes as argument
#PWD := $(shell pwd)

#CROSS_COMPILE	=arm-linux-
CC	=arm-linux-gcc

OBJS_HMC=hmc5983.c 

hmc5983:$(OBJS_HMC)
	$(CC) $^  -o   $@  -lm

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions

#.PHONY: modules modules_install clean



