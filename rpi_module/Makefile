obj-m := our_module.o 
our_module-y := module.o 

EXTRA_CFLAGS = -I$(PWD)

ARCH ?= arm
CROSS_COMPILE ?= $(HOME)/tools/arm-bcm2708/arm-linux-gnueabihf/bin/arm-linux-gnueabihf-
KDIR ?= $(HOME)/linux 

default:
	$(MAKE) -C $(KDIR) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) modules

clean: 
	$(MAKE) -C $(KDIR) M=$(PWD) clean 
