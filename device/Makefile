KERNEL_SOURCE := /usr/src/linux
#KERNEL_SOURCE := /lib/modules/3.13.0-32-generic/build
PWD := $(shell pwd)

obj-m += tyrion.o

default: module

module:
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} modules

clean:
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} clean

socfpga.dtb : socfpga.dtb
	${KERNEL_SOURCE}/scripts/dtc/dtc -O dtb -o socfpga.dtb socfpga.dts
