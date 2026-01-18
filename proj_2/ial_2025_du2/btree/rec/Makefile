CC=gcc
CFLAGS=-Wall -std=c11 -pedantic -lm
FILES=btree.c ../btree.c ../test_util.c ../test.c

.PHONY: test clean

test: $(FILES)
	$(CC) $(CFLAGS) -o $@ $(FILES)

clean:
	rm -f test

valgrind: test
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./test