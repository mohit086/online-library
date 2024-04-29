#include "../headers/codes.h"
#include "../headers/server_utilities.h"

// returns index of the client if it exists
int get_client(char *username){
    for (int i = 0; i < MAX_CLIENTS; i++) if (strcmp(client_arr[i].name, username) == 0) return i;
    return -1;
}

// adds a new user to the list
int add_user(char *username){
    if (get_client(username) == -1){
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (client_arr[i].name[0] == '\0'){
                strcpy(client_arr[i].name, username);
                client_arr[i].is_online = 1;
                break;
            }
        }
    }
    return 0;
}

// handles ctrl + c to avoid socket bind even after exit
void signal_handler(int signum){
    if (signum == SIGINT){
        unlink("/tmp/socket");
        exit(0);
    }
}

// server side authentication
void server_side_authenticate(int* sock, char* auth_request, char* username, char* password,int choice,  char* auth_response){
    while (1){
        if (read(*sock, auth_request, MSG_SIZE) <= 0) return;
        sscanf(auth_request, "AUTH %d %[^/]/%s", &choice, username, password);

        int fd = open("../db/user_db.txt", O_RDONLY);
        if (fd == -1){
            perror("DATABASE_ERROR");
            return;
        };
        char line[MSG_SIZE], stored_username[CRED_SIZE], stored_password[CRED_SIZE];

        int username_exists = 0, i = 0;
        char c;
        while (read(fd, &c, 1) > 0) {
            if (c == '\n' || i >= MSG_SIZE - 1) {
                line[i] = '\0';
                i = 0;
                sscanf(line, "%[^/]/%s", stored_username, stored_password);
                if (strcmp(username, stored_username) == 0) {
                    username_exists = 1;
                    break;
                }
            } 
            else line[i++] = c;
        }

        if (choice == 0){
            if (username_exists) strcpy(auth_response, "USERNAME EXISTS");
            else {
                fd = open("../db/user_db.txt", O_APPEND);
                dprintf(fd, "\n%s/%s", username, password);
                close(fd);
                add_user(username);
                strcpy(auth_response, "AUTH_SUCCESS");
            }
        }

        else if (choice == 1){
            if (username_exists){
                if (strcmp(password, stored_password) == 0) {
                    if (client_arr[get_client(username)].is_online) strcpy(auth_response, "ALREADY LOGGED IN"); // if already logged in
                    else {
                        strcpy(auth_response, "AUTH_SUCCESS");
                        client_arr[get_client(username)].is_online = 1;
                    }
                }
                else strcpy(auth_response, "INVALID PASSWORD");
            }
            else strcpy(auth_response, "USERNAME NOT FOUND");
        }  
        send(*sock, auth_response, MSG_SIZE, 0);
        if (strcmp(auth_response, "AUTH_SUCCESS") == 0){
            printf("%s CONNECTED\n\n", username);
            break;
        }
    }
}

// adds a book to the database
// int add_book(int book_id, char* title, char* author, int quantity){
//     int fd = open("../db/book_db.txt",O_APPEND);
//     dprintf(fd, "%d/%s/%s/%d\n", book_id, title, author, quantity);
//     close(fd);
//     return 0;
// }