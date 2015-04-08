CARGO ?= /usr/local/share/cargo
QSIM ?= ${CARGO}/qsim
KSRC ?= $(QSIM)/linux
CFLAGS := -O3
CFLAGS += -m32 -static
CFLAGS += -I../.. -I../../dev/src -I../
LIBS = -lm -lrt

all: tarball

check:
ifeq ($(KSRC),)
	$(error 'Path to kernel in env variable KSRC not found. Exiting')
endif
.PHONY: check

svd-mmap: svd-mmap.c
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

tarball: svd.tar
.PHONY: tarball

svd.tar: svd-mmap svd.ko runme.sh
	tar cf $@ $^

driver: svd.ko

.PHONY: driver

svd.ko: check
	make -C $(KSRC) M=`pwd`

clean help: check
	$(MAKE) -C $(KSRC) M=`pwd` $@
distclean: clean
	$(RM) svd.tar svd-mmap

.PHONY: all clean help
