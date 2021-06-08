
CC=clang
CFLAGS=-Wall -Wextra -Werror -pedantic -std=c17 -O2 -g -fsanitize=address
LFLAGS=-g -fsanitize=address

OBJS=panic.o endian.o address.o message.o
CLIENT=client.o
SERVER=server.o

client: $(OBJS) $(CLIENT)
	$(CC) $(LFLAGS) -o $@ $(OBJS) $(CLIENT)
server: $(OBJS) $(SERVER)
	$(CC) $(LFLAGS) -o $@ $(OBJS) $(SERVER)
.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<
clean:
	rm -rf *.o client server
.PHONY: clean

