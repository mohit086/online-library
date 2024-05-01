CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS = -lpthread

SRCDIR = src
BINDIR = bin

SERVER_SRC = $(SRCDIR)/server.c $(SRCDIR)/utils.c
CLIENT_SRC = $(SRCDIR)/client.c
DBCLEAR_SRC = $(SRCDIR)/clear_db.c

SERVER = $(BINDIR)/server
CLIENT = $(BINDIR)/client
DBCLEAR = $(BINDIR)/dbclear

all: $(SERVER) $(CLIENT) $(DBCLEAR)

$(SERVER): $(SERVER_SRC)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

$(CLIENT): $(CLIENT_SRC)
	$(CC) $(CFLAGS) $^ -o $@

$(DBCLEAR): $(DBCLEAR_SRC)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f $(SERVER) $(CLIENT) $(DBCLEAR)