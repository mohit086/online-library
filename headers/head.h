#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
// #include <stdbool.h>
#include <arpa/inet.h>

#define MSG_SIZE 1024
#define CRED_SIZE 32
#define MAX_CLIENTS 10
#define PORT 8080
#define TITLE_SIZE 64
#define AUTHOR_SIZE 64

// Structure to store client information
struct On_status{
    char name[CRED_SIZE];
    int is_online;
};

// structure for user
struct User {
    char username[CRED_SIZE];
    char password[CRED_SIZE];
    int valid;
};

// structure for book
struct Book{
    int id;
    char title[TITLE_SIZE];
    char author[AUTHOR_SIZE];
    int quantity;
    int valid;
};

// structure for book issue
struct Issue {
    char user[CRED_SIZE];
    char book_name[TITLE_SIZE];
};

// Array to store all users and their online status
extern struct On_status online_arr[MAX_CLIENTS];