CC=gcc
CFLAGS=-I.
DEPS=rbtree.h 
OBJ=rbtree.o rbtest.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

rbtest: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	-rm -f rbtest $(OBJ)
