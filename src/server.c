#include "../headers/codes.h"
#include "../headers/server_utilities.h"
client client_arr[MAX_CLIENTS];

// handles client operations (Thread function)
void* client_handler(void* sockfd){
    int sock = *((int *)sockfd), choice;
    char auth_request[MSG_SIZE], auth_response[MSG_SIZE], username[CRED_SIZE], password[CRED_SIZE];
    server_side_authenticate(&sock, auth_request, username, password, choice, auth_response);

    while (1){
        memset(auth_request, 0, sizeof(auth_request));
        if (read(sock, auth_request, MSG_SIZE) == 0){
            printf("%s LOGGED OUT\n\n", username);
            client_arr[get_client(username)].is_online = 0;
            break;
        }

        char str1[MSG_SIZE], str2[MSG_SIZE], str3[MSG_SIZE];
        int num1, num2;
        sscanf(auth_request, "%[^/]/%d/%[^/]/%[^/]/%d", str1, &num1, str2, str3, &num2);

        if (strcmp(str1,"ADDBOOK")==0) add_book(num1,str2,str3,num2);

        printf("%s: %s\n", username, auth_request);
        write(sock, "OK", strlen("OK"));
        printf("server: %s\n\n", "OK");
    }

    close(sock);
    return NULL;
}

int main(){
    memset(client_arr, 0, sizeof(client_arr));
    int server_fd, sock;
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){ // IPv4 TCP-IP Socket
        perror("\nSOCKET FAILED\n");
        return -1;
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == -1) { // Bind socket to address
        perror("\nBIND FAILED\n");
        return -1;
    }
    signal(SIGINT, signal_handler);

    if (listen(server_fd, MAX_CLIENTS) < 0) { // Listen for connections
        perror("\nLISTEN FAILED\n");
        return -1;
    }
    printf("LIBRARY SERVER STARTED\n\n");
    for (int i = 0; i < MAX_CLIENTS; i++) client_arr[i].is_online = false; // Initialize client array

    while (1){ // Accept connections in a loop with a new thread for each connection
        if ((sock = accept(server_fd, (struct sockaddr *)&address, &addrlen)) == -1){
            perror("\nACCEPT FAILED\n");
            return -1;
        }
        pthread_t thrd;
        pthread_create(&thrd, NULL, client_handler, (void *)&sock);
        pthread_detach(thrd);
    }
    return 0;
}