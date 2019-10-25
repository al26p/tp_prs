CC=gcc
CFLAGS=-Wall -g

SRC=			\
	client.c \
	server.c

OBJ=$(SRC:.c=.o)

BIN= \
	client \
	server

$(BIN): $(OBJ)
	$(CC) $(OBJ) -o $@

%.o:%.c
	$(CC) -c $(CFLAGS) $<

clean:
	rm -f $(OBJ) $(BIN)
