#include "../headers/head.h"
#include "../headers/utilities.h"

// returns index of the client if it exists
int get_client(char *username){
    for (int i = 0; i < MAX_CLIENTS; i++) if (strcmp(online_arr[i].name, username) == 0) return i;
    return -1;
}

// adds a new user to the online array
int add_user(char *username){
    if (get_client(username) == -1){
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (online_arr[i].name[0] == '\0'){
                strcpy(online_arr[i].name, username);
                online_arr[i].is_online = 1;
                break;
            }
        }
    }
    return 0;
}

// server side authentication
void server_side_authenticate(int* sock, char* auth_request, char* auth_response, struct User* user){
    while (1){
        char username[CRED_SIZE];
        char password[CRED_SIZE];
        int choice, username_exists = 0, on;

        if (read(*sock, auth_request, MSG_SIZE) <= 0) return;
        sscanf(auth_request, "AUTH %d %[^/]/%s", &choice, username, password);

        int fd = open("../db/user_db.dat", O_RDONLY);
        if (fd == -1){
            perror("DATABASE_ERROR");
            return;
        };
        struct User temp;
        while(read(fd,&temp,sizeof(struct User))){
            if (!strcmp(temp.username,username)){
                username_exists = 1;
                break;
            }
        }
        close(fd);
        on = online_arr[get_client(username)].is_online;

        if (choice == 0){
            if (username_exists) strcpy(auth_response, "USERNAME EXISTS");
            else {
                fd = open("../db/user_db.dat", O_RDWR|O_APPEND, 0666);
                strcpy(user->username,username);
                strcpy(user->password,password);
                user->valid = 1;
                write(fd,user,sizeof(struct User));
                close(fd);
                add_user(username);
                strcpy(auth_response, "AUTH_SUCCESS");
            }
        }

        else if (choice == 1){
            if (username_exists && temp.valid == 1){
                if (strcmp(password, temp.password) == 0){
                    if (on) strcpy(auth_response, "ALREADY LOGGED IN");
                    else {
                        strcpy(auth_response, "AUTH_SUCCESS");
                        online_arr[get_client(username)].is_online = 1;
                        strcpy(user->username,temp.username);
                        strcpy(user->password,temp.password);
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