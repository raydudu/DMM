# export ARCH=/usr/local/armhf/r26/bin/arm-axis-linux-gnueabihf-
#
# export CC=$(ARCH)gcc
# export AS=$(ARCH)as
# export LD=$(ARCH)ld

#for ARM APCS frame pointers:
# CFLAGS+=-mapcs-frame
CFLAGS += -fno-omit-frame-pointer

CFLAGS+=-fPIC -Wall -ggdb3 -Iexternal
MY_LDFLAGS=-fPIC -shared -ldl -ggdb3

OBJ-y=dmm.o backtrace.o tlocker.o hooks_stdlib.o symhelper.o allocdb.o \
	external/elfhacks.o external/htable.o

PHONY+=all
all: libdmm.so

libdmm.so: $(OBJ-y)
	$(CC) $(MY_LDFLAGS) $(LDFLAGS) -o $@ $^

PHONY+=tests
tests:
	$(MAKE) -C tests

PHONY+=clean
clean:
	rm -f *.o external/*.o libdmm.so

PHONY+=distclean
distclean: clean
	$(MAKE) -C tests clean

.PHONY: $(PHONY)
