#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>
#include <stdbool.h>
#include "codes.h"

// Structure to store client information
typedef struct {
    int sock;
    char username[CRED_SIZE];
    bool is_logged_in;
} client;

client client_arr[MAX_CLIENTS]; // Array to store client information

// Function to find client index by username
int find_client_index(char *username) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_arr[i].is_logged_in && strcmp(client_arr[i].username, username) == 0) {
            return i;
        }
    }
    return -1;
}

// Function to handle client authentication
int client_authenticate(char *username, char *password, int choice, char *auth_response) {
    FILE *file = fopen("user_db.txt", "r");
    if (file == NULL) return DATABASE_ERROR;

    char line[MSG_SIZE], stored_username[CRED_SIZE], stored_password[CRED_SIZE];
    int username_exists = 0;

    while (fgets(line, sizeof(line), file)) {
        sscanf(line, "%[^:]:%s", stored_username, stored_password);
        if (strcmp(username, stored_username) == 0){
            username_exists = 1;
            break;
        }
    }
    fclose(file);

    if (choice == 0){
        if (username_exists) strcpy(auth_response, "USERNAME EXISTS");
        else {
            file = fopen("user_db.txt", "a");
            fprintf(file, "\n%s:%s", username, password);
            fclose(file);
            strcpy(auth_response, "AUTH_SUCCESS");
        }
    } 
    else {
        if (username_exists){
            if (strcmp(password, stored_password) == 0) {
                if (find_client_index(username) != -1) strcpy(auth_response, "ALREADY_LOGGED_IN");
                else strcpy(auth_response, "AUTH_SUCCESS");
            }
            else strcpy(auth_response, "INVALID PASSWORD");
        }
        else strcpy(auth_response, "USERNAME NOT FOUND");
    }
    return AUTH_DONE;
}

void* client_handler(void* sockfd) {
    int sock = *((int *)sockfd), choice;
    char auth_request[MSG_SIZE], auth_response[MSG_SIZE], username[CRED_SIZE], password[CRED_SIZE];

    while (1){ // Authentication process
        if (read(sock, auth_request, MSG_SIZE) <= 0) return NULL;
        sscanf(auth_request, "AUTH %d %[^:]:%s", &choice, username, password);
        if (client_authenticate(username, password, choice, auth_response) == DATABASE_ERROR) {
            perror("DATABASE ERROR");
        }
        send(sock, auth_response, sizeof(auth_response), 0);
        if (strcmp(auth_response, "AUTH_SUCCESS") == 0) {
            int index = find_client_index(username);
            if (index != -1) {
                strcpy(auth_response, "ALREADY_LOGGED_IN");
                send(sock, auth_response, sizeof(auth_response), 0);
                close(sock);
                return NULL;
            } else {
                // Add client information
                for (int i = 0; i < MAX_CLIENTS; i++) {
                    if (!client_arr[i].is_logged_in) {
                        client_arr[i].sock = sock;
                        strcpy(client_arr[i].username, username);
                        client_arr[i].is_logged_in = true;
                        break;
                    }
                }
                if (strcmp(username, "admin") == 0) printf("ADMIN LOGIN\n\n");
                else printf("CLIENT %s CONNECTED\n\n", username);
                break;
            }
        }
    }

    while (1){ // Operation process
        memset(auth_request, 0, sizeof(auth_request));
        if (read(sock, auth_request, MSG_SIZE) == 0 || strcmp(auth_request, "logout") == 0) {
            int index = find_client_index(username);
            if (index != -1) {
                client_arr[index].is_logged_in = false;
            }
            printf("%s LOGGED OUT\n", username);
            break;
        }
        printf("%s: %s\n", username, auth_request);
        write(sock, "OK", strlen("OK"));
        printf("server: %s\n\n", "OK");
    }

    close(sock);
    return NULL;
}

int main() {
    int server_fd, sock;
    struct sockaddr address;
    socklen_t addrlen = sizeof(address);

    if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == 0) { // unix domain stream socket with default protocol 
        perror("\nSOCKET FAILED\n");
        return -1;
    }
    unlink("/tmp/socket"); // unlinking old socket file if it exists

    address.sa_family = AF_UNIX; // setting up server address
    strcpy(address.sa_data, "/tmp/socket");

    if (bind(server_fd, &address, addrlen) == -1) { // bind socket to that address
        perror("\nBIND FAILED\n");
        return -1;
    }

    if (listen(server_fd, MAX_CONNECTIONS) < 0) { // listen for connections
        perror("\nLISTEN FAILED\n");
        return -1;
    }
    printf("LIBRARY SERVER STARTED\n\n");
    for (int i = 0; i < MAX_CLIENTS; i++) client_arr[i].is_logged_in = false; // initialise client array

    while (1){ // accept connections in a loop with a new thread for each connection
        if ((sock = accept(server_fd, &address, &addrlen)) == -1) {
            perror("\nACCEPT FAILED\n");
            return -1;
        }
        pthread_t thrd;
        pthread_create(&thrd, NULL, client_handler, (void *)&sock);
        pthread_detach(thrd);
    }

    return 0;
}