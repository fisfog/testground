CC=gcc
DEPS=
CFLAGS=-g
LFLAGS=-L. -lz

zlibtest: zlibtest.c
	$(CC) -o $@ $(CFLAGS) $^ $(LFLAGS)

.PHONY: clean

clean:
	-rm -f zlibtest
