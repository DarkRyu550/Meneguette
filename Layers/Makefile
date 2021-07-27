CFLAGS=-Wall -Wextra -Werror -pedantic -std=c17 -O2 -g -fsanitize=address,undefined
LFLAGS=-g -fsanitize=address,undefined -rdynamic

OBJS=bit_vector.o bitconv.o crc32.o error_check.o main.o panic.o parity.o

all: layers

layers: $(OBJS)
	$(CC) $(LFLAGS) -o $@ $(OBJS)
.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<
clean:
	rm -rf *.o layers

run: layers
	./layers

.PHONY: clean all run

