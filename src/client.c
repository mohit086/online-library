#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include "codes.h"

void authenticate(int* sock){
    char username[CRED_SIZE], password[CRED_SIZE], auth_request[MSG_SIZE], auth_response[MSG_SIZE];
    int choice, is_authenticated = 0;

    while(!is_authenticated){
        memset(username, 0, CRED_SIZE); // clear all buffers
        memset(password, 0, CRED_SIZE);
        memset(auth_response, 0, MSG_SIZE);

        while(1){ // initial choice of login/signup
            printf("Enter 0 to signup, 1 to login: ");
            if (scanf("%d", &choice) != 1 || (choice != 0 && choice != 1)){
                while (getchar() != '\n');
                continue;
            }
            break;
        }

        printf("\nEnter username: ");
        scanf("%s", username);
        printf("Enter password: ");
        scanf("%s", password);
        sprintf(auth_request, "AUTH %d %s:%s", choice, username, password);

        send(*sock, auth_request, MSG_SIZE, 0); //send auth request to server
        read(*sock, auth_response, MSG_SIZE); // receive server response

        if(strcmp(auth_response,"AUTH_SUCCESS")==0){ // if authentication successful
            is_authenticated = 1;
            if (strcmp(username,"admin")==0) printf("LOGGED IN AS ADMIN\n\n");
            else printf("\nAuthenticated successfully as %s.\n\n",username);
        }
        else printf("AUTHENTICATION ERROR - %s.\n\n",auth_response); // If not
    }
    return;
}

void operate(int* sock){
    char oper_request[MSG_SIZE], oper_response[MSG_SIZE];
    while(1){
        memset(oper_response, 0, MSG_SIZE); // clear the previous server response
        printf("Enter message: ");
        scanf(" %[^\n]",oper_request);

        if (strcmp(oper_request, "logout") == 0){ // if message is logout, log out
            send(*sock, oper_request, MSG_SIZE, 0);
            printf("\nLogged Out.\n\n");
            break;
        }

        send(*sock, oper_request, MSG_SIZE, 0); // send message to server
        read(*sock, oper_response, MSG_SIZE); // receive server response
        printf("Server Response: %s\n\n", oper_response);
    }
    return;
}

int main(){
    int sock = 0;
    struct sockaddr client_address;
    if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) < 0){ // UNIX domain stream socket with default protocol
        perror("SOCKET FAILED");
        return -1;
    }
    client_address.sa_family = AF_UNIX;
    strcpy(client_address.sa_data, "/tmp/socket"); // copy the socket path to the structure
    if (connect(sock, &client_address, sizeof(client_address)) < 0){
        perror("CONNECTION FAILED");
        return -1;
    }

    authenticate(&sock); // User authentication
    operate(&sock); // Handling operateions

    close(sock); // close client side socket
    return 0;
}