all: cargo

CARGO ?= /usr/local/share/cargo
QSIM ?= ${CARGO}/qsim

SVD_OBJS = svd-sync.o device-list.o
SVD_CXX_OBJS = svd.opp svd-main.opp svd-wrapper.opp
HDRS = svd.h  svd-wrapper.hpp  svd-sync.h svd_data.h

SVD_DIR := $(dir $(lastword $(MAKEFILE_LIST)))
CARGO_DEV_OBJS += $(addprefix $(SVD_DIR),$(SVD_OBJS))
CARGO_DEV_CXX_OBJS += $(addprefix $(SVD_DIR),$(SVD_CXX_OBJS))

QSIM ?= qsim
CCAN ?= $(QSIM)/ccan
LUA ?= lua
MCPAT ?= mcpat
SYSTEMC_PATH := $(CARGO)/systemc
SYSTEMC ?= $(SYSTEMC_PATH)/installed

QSIM_LIB := $(QSIM)/lib/libqsim.a
CCAN_LIB := $(CCAN)/libccan.a
LUA_LIB := $(LUA)/lib/liblua.a
MCPAT_LIB := $(MCPAT)/lib/libmcpat.a
SYSTEMC_LIB := $(SYSTEMC)/lib-linux64/libsystemc.a
CSP_LIB := $(CARGO)/csp/lib.a

DEP_LIBS :=

# cargo objs depend on DEP_LIBS, but only the cargo binary depends on ALL_LIBS
ALL_LIBS :=
ALL_LIBS += $(DEP_LIBS)
ALL_LIBS += $(CSP_LIB)

INCLUDE_FLAGS :=
INCLUDE_FLAGS += -I.
INCLUDE_FLAGS += -I$(CARGO)
INCLUDE_FLAGS += -I$(CCAN)
INCLUDE_FLAGS += -I$(QSIM)/include
INCLUDE_FLAGS += -I$(CARGO)/$(LUA)/include
INCLUDE_FLAGS += -I$(CARGO)/$(MCPAT)/include
INCLUDE_FLAGS += -I$(SYSTEMC)/include

CFLAGS = -g3 -ggdb -O0
CFLAGS ?=
include ../svd/sim/Makefile.params
ALL_CFLAGS :=
ALL_CFLAGS += $(CFLAGS)
ALL_CFLAGS += $(INCLUDE_FLAGS)
ALL_CFLAGS += $(LUA_CFLAGS)
ALL_CFLAGS += -fms-extensions
ALL_CFLAGS += -Wall
ALL_CFLAGS += -Wextra
ALL_CFLAGS += -Wmissing-declarations
ALL_CFLAGS += -Wno-missing-field-initializers
ALL_CFLAGS += -Wno-unused-parameter
ALL_CFLAGS += -Wpointer-arith
ALL_CFLAGS += -Wundef
ALL_CXXFLAGS := $(ALL_CFLAGS)
# C-only flags
ALL_CFLAGS += -Wold-style-definition
ALL_CFLAGS += -Wmissing-prototypes
ALL_CFLAGS += -Wstrict-prototypes

ALL_CXXFLAGS += -Wno-unused-label
ALL_CXXFLAGS += -DSC_INCLUDE_FX

LDFLAGS :=
LDFLAGS += -L$(CCAN)
LDFLAGS += -L$(QSIM)/lib
LDFLAGS += -L$(CARGO)/$(LUA)/lib
LDFLAGS += -L$(CARGO)/$(MCPAT)/lib
LDFLAGS += -L$(SYSTEMC)/lib-linux64

PROFILE_LDFLAGS := -L$(CCAN)

LIBS :=
LIBS += -lccan
LIBS += -lqsim
LIBS += -llua
LIBS += -lmcpat
# libstdc++ needed by mcpat
LIBS += -lstdc++
LIBS += -l:libsystemc.a
LIBS += -lpthread
LIBS += -ldl
LIBS += -lrt
LIBS += -lm
LIBS += $(CARGO)/csp/lib.a

OBJECTS :=
OBJECTS += $(CARGO_OBJS) $(CARGO_DEV_OBJS)
OBJECTS += $(CSP_OBJS)

CXX_OBJECTS :=
CXX_OBJECTS += $(CARGO_DEV_CXX_OBJS)

C_OBJECTS := $(OBJECTS)
ALL_OBJECTS := $(C_OBJECTS) $(CXX_OBJECTS)

CC ?= gcc
CXX := g++
CHECK := sparse

ifndef V
	QUIET_AR            = @echo '   ' AR $@;
	QUIET_BUILD         = @echo '   ' BUILD $@;
	QUIET_CC            = @echo '   ' CC $@;
	QUIET_CXX           = @echo '   ' CXX $@;
	QUIET_CHECKPATCH    = @echo '   ' CHECKPATCH $(subst .o,.c,$@);
	QUIET_CHECK         = @echo '   ' CHECK $(subst .o,.c,$@);
	QUIET_LINK          = @echo '   ' LINK $@;
endif

# objects depend on libraries we build so that header files can be found
$(C_OBJECTS): %.o: %.c $(missing_dep_dirs) $(DEP_LIBS)
	$(QUIET_CC)$(CC) -g3  -o $*.o -c $(dep_args) $(ALL_CFLAGS) $<

$(CXX_OBJECTS): %.opp: %.cpp $(missing_dep_dirs) $(DEP_LIBS)
	$(QUIET_CXX)$(CXX) -g3 $(ALL_CXXFLAGS) -o $*.opp -c $(dep_args) $<

cargo: $(CARGO_DEV_OBJS) $(CARGO_DEV_CXX_OBJS) $(ALL_LIBS) $(HDRS)
	$(QUIET_LINK)$(CC) -g3 -o$@ $(LDFLAGS) $(CARGO)/libcargo.a $(filter %.opp,$^)  $(filter %.o,$^) $(LIBS)


run: cargo
	@make -C ./test
	@./runcargo svd

.PHONY:

clean:
	$(RM) $(ALL_OBJECTS)
	$(RM) cargo
	$(RM) CARGO-CFLAGS CARGO-LDFLAGS
	$(RM) cargo.power* cargo.data*
	make -C ./test distclean

.PHONY: all clean
