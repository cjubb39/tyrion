SYSTEMC ?= /opt/lib/systemc-2.2.0
TLM ?= /opt/lib/TLM-2009-07-15
TARGET_ARCH := linux64
WAVE_VIEWER := simvision
CC := g++

CFLAGS ?=
include Makefile.params # after CFLAGS defined

PARAMS_FILE=.params

LIBS   := -lsystemc
LIBDIR = -L. -L$(SYSTEMC)/lib-$(TARGET_ARCH)
CTOS_ROOT := $(CTOS_PATH)
CTOS_INCLUDES := -I$(CTOS_ROOT)/share/ctos/include \
                -I$(CTOS_ROOT)/share/ctos/include/ctos_fx \
                -I$(CTOS_ROOT)/share/ctos/include/ctos_tlm \
                -I$(CTOS_ROOT)/share/ctos/include/ctos_flex_channels
MATH = /opt/zynq-math/src/

INCDIR = -I../src -I../tb -I$(SYSTEMC)/include -I$(TLM)/include/tlm \
-I$(DRAMSIM) $(CTOS_INCLUDES) -I$(MATH) -I/tools/ctos141/share/ctos/include

TARGET := utility
VPATH := ../src:../tb
SRCS := utility.cc
OBJS = $(SRCS:.cpp=.o)
HDRS := 

.SUFFIXES: .cpp .cc .o 
$(TARGET): $(OBJS)
	$(CC) ${INCDIR} $(CFLAGS) ${LIBDIR} -o $@ $(OBJS) $(C_OBJS) ${LIBS}

$(OBJS): $(HDRS) $(PARAMS_FILE)

.PHONY: force
$(PARAMS_FILE): force
	@echo '$(CFLAGS)' | cmp -s $@ || echo '$(CFLAGS)' > $@

.PHONY: clean distclean run wave

.cpp.o:
	$(CC) ${INCDIR} $(CFLAGS) -c $<

clean:
	rm -f *.o $(TARGET) *.dsn *.trn *.vcd $(PARAMS_FILE)
	if [ -d .simvision ]; then rm -r .simvision; fi

distclean: clean
	rm -rf results
	rm -rf *.log

run: $(TARGET)
	rm -f core
	./$(TARGET)

wave:
	$(WAVE_VIEWER) $(TARGET).vcd
