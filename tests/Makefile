CFLAGS+=-ggdb3 -fno-omit-frame-pointer
LDFLAGS+=-ggdb3

TESTS=prog1 prog2 prog3 myalloc1 htable_test backtrace_test json2f_test tls_verify
all: $(TESTS)

htable.o: ../external/htable.c
	$(CC) $(CFLAGS) -c -o $@ $^

backtrace.o: ../backtrace.c
	$(CC) $(CFLAGS) -c -o $@ $^

prog3: prog3.o
	$(CC) -lpthread -o $@ $^ $(LDFLAGS)

tls_verify: tls_verify.o
	$(CC) -lpthread -o $@ $^ $(LDFLAGS)



htable_test: htable_test.o htable.o
	$(CC) $(LDFLAGS) -o $@ $^

backtrace_test: backtrace_test.o backtrace.o
	$(CC) $(LDFLAGS) -o $@ $^

clean:
	rm -f $(TESTS) *.o

list_tests:
	@echo $(TESTS)
