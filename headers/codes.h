#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/un.h>
#include <signal.h>
#include <pthread.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define MSG_SIZE 1024
#define CRED_SIZE 64
#define MAX_CONNECTIONS 10
#define MAX_CLIENTS 10
#define PORT 8080
#define TITLE_SIZE 64
#define AUTHOR_SIZE 64