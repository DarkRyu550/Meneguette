
CC=clang
CFLAGS=-Wall -Wextra -Werror -pedantic -std=c17 -O2 -g -fsanitize=address,undefined
LFLAGS=-g -fsanitize=address,undefined

OBJS=panic.o endian.o address.o message.o
CLIENT=client.o
SERVER=server.o message_board.o

all: client server

client: $(OBJS) $(CLIENT)
	$(CC) $(LFLAGS) -o $@ $(OBJS) $(CLIENT)
server: $(OBJS) $(SERVER)
	$(CC) $(LFLAGS) -o $@ $(OBJS) $(SERVER)
.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<
clean:
	rm -rf *.o client server

run-client: client
	./client c1
run-server: server
	./server

.PHONY: clean all

