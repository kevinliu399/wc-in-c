CC = gcc
CFLAGS = -Wall -Wextra -std=c99

all: wwc test_wwc

wwc: wwc.c
	$(CC) $(CFLAGS) -o wwc wwc.c

test_wwc: test_wwc.c
	$(CC) $(CFLAGS) -o test_wwc test_wwc.c

test: wwc test_wwc
	./test_wwc

clean:
	rm -f wwc test_wwc *.o

.PHONY: all test clean