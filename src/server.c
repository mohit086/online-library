#include "../headers/head.h"
#include "../headers/utilities.h"
On_status online_arr[MAX_CLIENTS];

// handles client operations (Thread function)
void* client_handler(void* sockfd){
    int sock = *((int *)sockfd), choice;
    char request[MSG_SIZE], response[MSG_SIZE];
    User* u = (User*)malloc(sizeof(User));
    server_side_authenticate(&sock, request, response, u);
    printf("%s %s\n",u->username, u->password);

    while (1){
        memset(request, 0, sizeof(request));
        if (read(sock, request, MSG_SIZE) == 0){
            printf("%s LOGGED OUT\n\n", u->username);
            online_arr[get_client(u->username)].is_online = 0;
            break;
        }
        char cmd[MSG_SIZE], s1[MSG_SIZE], s2[MSG_SIZE];
        int n1, n2;
        sscanf(request, "%[^/]/%d/%[^/]/%[^/]/%d", cmd, &n1, s1, s2, &n2);

        if (strcmp(cmd,"ADBOOK")==0) add_book(n1,s1,s2,n2,response);
        if (strcmp(cmd,"RMBOOK")==0) remove_book(n1,response);
        if (strcmp(cmd,"CHBQTY")==0) change_qty(n1,n2,response);
        if (strcmp(cmd,"RMUSER")==0) remove_user(s1,response);
        if (strcmp(cmd,"VISUBS")==0) view_all_issues(response);
        if (strcmp(cmd,"VAVBKS")==0) view_avl_books(response);
        if (strcmp(cmd,"MYBOOK")==0) view_mybooks(u->username, response);
        if (strcmp(cmd,"ISSUBK")==0) issue_book(u->username, n1, response);
        if (strcmp(cmd,"RTRNBK")==0) return_book(u->username, n1, response);
        if (strcmp(cmd,"CHGPWD")==0) change_password(u->username, s1, s2, response);
        if (strcmp(cmd,"AADMIN")==0) add_admin(s1,s2,response);
        if (strcmp(cmd,"LOGOUT")==0){
            printf("%s LOGGED OUT\n\n", u->username);
            online_arr[get_client(u->username)].is_online = 0;
            break;
        }
        printf("%s: %s\n", u->username, request);
        write(sock,response,MSG_SIZE);
        printf("server: %s\n\n", response);
    }

    close(sock);
    return NULL;
}

int main(){
    memset(online_arr, 0, sizeof(online_arr));
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
        perror("BIND FAILED");
        return -1;
    }
    if (listen(server_fd, MAX_CLIENTS) < 0) { // Listen for connections
        perror("LISTEN FAILED");
        return -1;
    }
    printf("LIBRARY SERVER STARTED\n\n");
    for (int i = 0; i < MAX_CLIENTS; i++) online_arr[i].is_online = 0; // Initialize client array
    while (1){ // Accept connections in a loop with a new thread for each connection
        if ((sock = accept(server_fd, (struct sockaddr *)&address, &addrlen)) == -1){
            perror("ACCEPT FAILED");
            return -1;
        }
        pthread_t thrd;
        pthread_create(&thrd, NULL, client_handler, (void *)&sock);
        pthread_detach(thrd);
    }
    return 0;
}