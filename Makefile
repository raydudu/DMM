#ARCH=/usr/local/armhf/r26/bin/arm-axis-linux-gnueabihf-


CC=$(ARCH)gcc
AS=$(ARCH)as
LD=$(ARCH)ld

#for AXIS ARM:
#CFLAGS+=-mapcs-frame

CFLAGS+=-fPIC -Wall -ggdb3 -Iexternal
LDFLAGS+=-fPIC -shared -ldl -ggdb3
ASFILES+=

all: libdmm.so
OBJ-y=dmm.o backtrace.o tlocker.o hooks_stdlib.o symhelper.o allocdb.o \
	external/elfhacks.o external/htable.o

libdmm.so: $(OBJ-y)
	$(CC) $(LDFLAGS) -o $@ $^

clean:
	rm -f *.o external/*.o libdmm.so
