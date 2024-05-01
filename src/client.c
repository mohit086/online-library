#include "../headers/head.h"
#include "../headers/client_utils.h"

int main(){
    int sock = 0;
    struct sockaddr_in server_address;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("SOCKET FAILED");
        return -1;
    }
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    if(inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr)<=0){
        perror("INVALID ADDRESS");
        return -1;
    }
    if (connect(sock, (struct sockaddr *)&server_address, sizeof(server_address))<0){
        perror("CONNECTION FAILED");
        return -1;
    }
    printf("Welcome to Online Library Management System\n\n");
    int is_admin = authenticate(&sock);
    operate(&sock, is_admin);
    close(sock);
    return 0;
}