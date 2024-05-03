#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/un.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <signal.h>
#include <arpa/inet.h>
#include <semaphore.h>
#include <sys/socket.h>

#define MSG_SIZE 1024
#define CRED_SIZE 64
#define MAX_CLIENTS 10
#define PORT 8081
#define TITLE_SIZE 64
#define AUTHOR_SIZE 64

// Structure to store client information
typedef struct{
    char name[CRED_SIZE];
    int is_online;
} On_status;

// structure for user
typedef struct {
    char username[CRED_SIZE];
    char password[CRED_SIZE];
    int is_admin;
    int valid;
} User;

// structure for book
typedef struct{
    int id;
    char title[TITLE_SIZE];
    char author[AUTHOR_SIZE];
    int quantity;
    int valid;
} Book;

// structure for book issue
typedef struct {
    char user[CRED_SIZE];
    int book_id;
    int valid;
} Issue;

// Array to store all users and their online status
extern On_status online_arr[MAX_CLIENTS];

// initialising semaphores for 3 files
extern sem_t book_sem;
extern sem_t user_sem;
extern sem_t issue_sem;