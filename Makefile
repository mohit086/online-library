CC = gcc
SRCDIR = src
BINDIR = bin
CFLAGS = -Wall -Wextra
LDFLAGS = -lpthread

SERVER_SRC = $(SRCDIR)/server.c $(SRCDIR)/server_utils.c
CLIENT_SRC = $(SRCDIR)/client.c $(SRCDIR)/client_utils.c
SETUP_SRC = $(SRCDIR)/setup.c

SERVER = $(BINDIR)/server
CLIENT = $(BINDIR)/client
SETUP = $(BINDIR)/setup

all: $(SERVER) $(CLIENT) $(SETUP)

$(SERVER): $(SERVER_SRC)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(CLIENT): $(CLIENT_SRC)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(SETUP): $(SETUP_SRC)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f $(SERVER) $(CLIENT) $(SETUP)