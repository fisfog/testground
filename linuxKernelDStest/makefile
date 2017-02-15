CC=gcc
CFLAGS=-I.
DEPS=rbtree.h list.h
OBJ=rbtree.o rbtest.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

rbtest: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.phony: clean

clean:
	-rm -f $(OBJ)
