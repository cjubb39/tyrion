SYSTEMC ?= /opt/lib/systemc-2.2.0
TLM ?= /opt/lib/TLM-2009-07-15
TARGET_ARCH := linux64
WAVE_VIEWER := simvision
CC := g++

CFLAGS ?= -g

PARAMS_FILE=.params

LIBS   := -lsystemc
LIBDIR = -L. -L$(SYSTEMC)/lib-$(TARGET_ARCH)
CTOS_ROOT := $(CTOS_PATH)
CTOS_INCLUDES := -I$(CTOS_ROOT)/share/ctos/include \
                -I$(CTOS_ROOT)/share/ctos/include/ctos_fx \
                -I$(CTOS_ROOT)/share/ctos/include/ctos_tlm \
                -I$(CTOS_ROOT)/share/ctos/include/ctos_flex_channels

INCDIR = -I../src -I../tb -I$(SYSTEMC)/include -I$(TLM)/include/tlm $(CTOS_INCLUDES)

TARGET := blargen 
VPATH := ../src:../tb
SRCS := blargen.cpp blargen_wrapper.cpp blargen_tb.cpp sc_main.cpp
OBJS = $(SRCS:.cpp=.o)
HDRS := blargen.h blargen_wrapper.h blargen_tb.h

.SUFFIXES: .cpp .cc .o
$(TARGET): $(OBJS)
	@rm -f core
	$(CC) ${INCDIR} $(CFLAGS) ${LIBDIR} -o $@ $(OBJS) ${LIBS}

$(OBJS): $(HDRS) $(PARAMS_FILE)

.PHONY: force
$(PARAMS_FILE): force
	@echo '$(CFLAGS)' | cmp -s $@ || echo '$(CFLAGS)' > $@

.PHONY: clean distclean run wave

.cpp.o:
	$(CC) ${INCDIR} $(CFLAGS) -c $<

clean:
	@rm -f *.o $(TARGET) *.dsn *.trn *.vcd $(PARAMS_FILE) *.log
	@rm -f traffic_stats_*.csv
	@if [ -d .simvision ]; then rm -r .simvision; fi

distclean: clean
	rm -rf results

run: $(TARGET)
	@rm -f core
	./$(TARGET)

wave:
	$(WAVE_VIEWER) $(TARGET).vcd
