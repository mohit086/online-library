#include "../headers/codes.h"

// Function declarations
int get_client(char *username);
int add_user(char *username);
void signal_handler(int signum);
void server_side_authenticate(int* sock, char* auth_request, char* username, char* password,int choice, char* auth_response);
int delete_line(FILE* fp);
int add_book(int id, char* title, char* author, int quantity);
int rm_book(int book_id);

// Structure to store client information
typedef struct {
    char name[CRED_SIZE];
    int is_online;
} client;

// Array to store all users and their online status
extern client client_arr[MAX_CLIENTS]; 