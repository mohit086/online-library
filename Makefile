CC = gcc
SRCDIR = src
BINDIR = bin

SERVER_SRC = $(SRCDIR)/server.c $(SRCDIR)/server_utils.c
CLIENT_SRC = $(SRCDIR)/client.c $(SRCDIR)/client_utils.c
SETUP_SRC = $(SRCDIR)/setup.c

SERVER = $(BINDIR)/server
CLIENT = $(BINDIR)/client
SETUP = $(BINDIR)/setup

all: $(SERVER) $(CLIENT) $(SETUP)

$(SERVER): $(SERVER_SRC)
	$(CC) $^ -o $@

$(CLIENT): $(CLIENT_SRC)
	$(CC) $^ -o $@

$(SETUP): $(SETUP_SRC)
	$(CC) $^ -o $@

clean:
	rm -f $(SERVER) $(CLIENT) $(SETUP)