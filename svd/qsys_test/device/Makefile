ifneq (${KERNELRELEASE},)

# KERNELRELEASE defined: we are being compiled as part of the Kernel
        obj-m := fifo0.o fifo1.o

else

# We are being compiled as a module: use the Kernel build system

	KERNEL_SOURCE := /usr/src/linux
        PWD := $(shell pwd)

default: module hello

module:
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} modules

clean:
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} clean
	${RM} hello
	${RM} *~

socfpga.dtb : socfpga.dtb
	/usr/src/linux/scripts/dtc/dtc -O dtb -o socfpga.dtb socfpga.dts

.PHONY: run
run:
	insmod fifo0.ko
	insmod fifo1.ko 
	./hello

.PHONY: all 
all: module hello run



endif 
