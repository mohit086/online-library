#include "../headers/codes.h"
#include <stdio.h>
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

int get_client(char *username);
int add_user(char *username);
void signal_handler(int signum);
void client_authenticate(char *username, char *password, int choice, char *auth_response);
void server_side_authenticate(int* sock, char* auth_request, char* username, char* password,int choice, char* auth_response);

// Structure to store client information
typedef struct {
    char name[CRED_SIZE];
    int is_online;
} client;

extern client client_arr[MAX_CLIENTS]; // Array to store all users and their online status