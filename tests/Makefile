#CFLAGS+=-mapcs-frame
CFLAGS+=-g
#LDFLAGS+=-mapcs-frame

TESTS=prog1 htable_test
all: $(TESTS)

htable.o: ../external/htable.c
	$(CC) $(CFLAGS) -c -o $@ $^

htable_test: htable_test.o htable.o
	$(CC) $(LDFLAGS) -o $@ $^
clean:
	rm -f $(TESTS) *.o
