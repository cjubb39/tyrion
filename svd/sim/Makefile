SYSTEMC ?= /opt/lib/systemc-2.2.0
TLM ?= /opt/lib/TLM-2009-07-15
DRAMSIM ?= ../../DRAMSim2/
DRAMSIM_LIB ?= libdramsim.so
TARGET_ARCH := linux64
WAVE_VIEWER := simvision
CC := g++

CFLAGS ?=
include Makefile.params # after CFLAGS defined

PARAMS_FILE=.params

LIBS   := -Wl,-rpath=$(DRAMSIM) -lsystemc -ldramsim
LIBDIR = -L. -L$(SYSTEMC)/lib-$(TARGET_ARCH) -L$(DRAMSIM)
CTOS_ROOT := $(CTOS_PATH)
CTOS_INCLUDES := -I$(CTOS_ROOT)/share/ctos/include \
                -I$(CTOS_ROOT)/share/ctos/include/ctos_fx \
                -I$(CTOS_ROOT)/share/ctos/include/ctos_tlm \
                -I$(CTOS_ROOT)/share/ctos/include/ctos_flex_channels

INCDIR = -I../src -I../tb -I$(SYSTEMC)/include -I$(TLM)/include/tlm -I$(DRAMSIM) $(CTOS_INCLUDES)

TARGET := fft2d
VPATH := ../src:../tb
SRCS := fft2d.cpp fft2d_tb.cpp sc_main.cpp memory_controller.cpp dramsim_controller.cpp
OBJS = $(SRCS:.cpp=.o)
HDRS := mydata.h fft2d.h fft2d_tb.h memory_controller.h memory_size.h dramsim_controller.h dramsim_config.h

.SUFFIXES: .cpp .cc .o
$(TARGET): $(DRAMSIM)/$(DRAMSIM_LIB) $(OBJS)
	$(CC) ${INCDIR} $(CFLAGS) ${LIBDIR} -o $@ $(OBJS) ${LIBS}

$(OBJS): $(HDRS) $(PARAMS_FILE)

.PHONY: force
$(PARAMS_FILE): force
	@echo '$(CFLAGS)' | cmp -s $@ || echo '$(CFLAGS)' > $@

$(DRAMSIM)/$(DRAMSIM_LIB):
	make -C $(DRAMSIM) $(DRAMSIM_LIB)

.PHONY: clean distclean run wave

.cpp.o:
	$(CC) ${INCDIR} $(CFLAGS) -c $<

clean:
	@rm -f *.o $(TARGET) *.dsn *.trn *.vcd $(PARAMS_FILE)
	@if [ -d .simvision ]; then rm -r .simvision; fi

distclean: clean
	make -C $(DRAMSIM) clean
	rm -rf results
	rm -rf *.log

run: $(TARGET)
	./$(TARGET)

wave:
	$(WAVE_VIEWER) $(TARGET).vcd
