CC=gcc
DEPS=
LFLAGS=-L. -lz

zlibtest: zlibtest.c
	$(CC) -o $@ $^ $(LFLAGS)

.PHONY: clean

clean:
	-rm -f zlibtest
