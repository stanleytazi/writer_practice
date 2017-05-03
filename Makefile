# Configurable options
#   MODE = release | debug (default: debug)
#   SNAPPY = 0 | 1 (default: 1)
#
CSTDFLAG = --std=c99 -pedantic -Wall -Wextra -Wno-unused-parameter
#CPPFLAGS += -fPIC -Iinclude -Iexternal/snappy
#CPPFLAGS += -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64
#CPPFLAGS += -D_XOPEN_SOURCE=500 -D_DARWIN_C_SOURCE
LDFLAGS += -lpthread
EXEC = testw
ifeq ($(MODE),release)
	CPPFLAGS += -O3
	DEFINES += -DNDEBUG
else
	CFLAGS += -g
endif

# run make with SNAPPY=0 to turn it off
ifneq ($(SNAPPY),0)
	DEFINES += -DBP_USE_SNAPPY=1
else
	DEFINES += -DBP_USE_SNAPPY=0
endif

all: $(EXEC)

external/snappy/config.status:
	(git submodule init && git submodule update && cd external/snappy)
	(cd external/snappy && ./autogen.sh && ./configure)

OBJS = writer.o

deps := $(OBJS:%.o=%.o.d)

writer.o: writer.c
	$(CC) $(CFLAGS) $(CSTDFLAG) $(CPPFLAGS) $(DEFINES) \
	    -o $@ -MMD -MF $@.d -c $<

testw: main.c writer.o
	$(CC) $(CFLAGS) $(CPPFLAGS) $< -o $@ writer.o $(LDFLAGS)

clean:
	@rm -f bplus.a
	@rm -f $(OBJS) $(TESTS) $(deps)

.PHONY: all check clean

-include $(deps)
