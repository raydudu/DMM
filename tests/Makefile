CFLAGS+=-ggdb3
LDFLAGS+=-ggdb3

TESTS=prog1 prog2 prog3 myalloc1 htable_test
all: $(TESTS)

htable.o: ../external/htable.c
	$(CC) $(CFLAGS) -c -o $@ $^

prog3: prog3.o
	$(CC) -lpthread -o $@ $^ $(LDFLAGS)
htable_test: htable_test.o htable.o
	$(CC) $(LDFLAGS) -o $@ $^


clean:
	rm -f $(TESTS) *.o
