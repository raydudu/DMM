CFLAGS+=-ggdb3
LDFLAGS+=-ggdb3

TESTS=prog1 prog2 myalloc1 htable_test
all: $(TESTS)

htable.o: ../external/htable.c
	$(CC) $(CFLAGS) -c -o $@ $^

htable_test: htable_test.o htable.o
	$(CC) $(LDFLAGS) -o $@ $^
clean:
	rm -f $(TESTS) *.o
