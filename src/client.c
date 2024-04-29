#include "../headers/codes.h"

int authenticate(int* sock){
    char username[CRED_SIZE], password[CRED_SIZE], auth_request[MSG_SIZE], auth_response[MSG_SIZE];
    int choice, is_authenticated = 0, is_admin = 0;
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
        sprintf(auth_request, "AUTH %d %s/%s", choice, username, password);
        send(*sock, auth_request, MSG_SIZE, 0); //send auth request to server
        read(*sock, auth_response, MSG_SIZE); // receive server response
        if(strcmp(auth_response,"AUTH_SUCCESS")==0){ // if authentication successful
            is_authenticated = 1;
            printf("\nAuthenticated successfully as %s.\n\n",username);
            if (strcmp(username,"admin")==0) is_admin = 1;
        }
        else printf("AUTHENTICATION ERROR - %s.\n\n",auth_response); // If not
    }
    return is_admin;
}

void operate(int* sock, int is_admin){
    char oper_request[MSG_SIZE], oper_response[MSG_SIZE];
    char title[CRED_SIZE], author[CRED_SIZE], username[CRED_SIZE], oldp[CRED_SIZE], newp[CRED_SIZE];
    int choice, quantity, book_id;

    if (!is_admin){
        printf("Choose any option number: \n");
        printf("1 - View available books\n");
        printf("2 - Search book by title\n");
        printf("3 - Search book by author\n");
        printf("4 - View my borrowed books\n");
        printf("5 - Issue a book\n");
        printf("6 - Return a book\n");
        printf("7 - Change password\n");
        printf("8 - Logout\n\n");
    }
    else {
        printf("Choose any option number: \n");
        printf("1 - Add a book\n");
        printf("2 - Remove a book\n");
        printf("3 - Change book details\n");
        printf("4 - Remove a user\n");
        printf("5 - View all borrowed books\n");
        printf("6 - Change password\n");
        printf("7 - Logout\n\n");
    }
    while(1){
        memset(oper_response, 0, MSG_SIZE); // clear the previous server response

        if (!is_admin){
            while(1){
                printf("Enter choice: ");
                scanf("%d",&choice);
                if (choice>0 && choice<9) break;
                else printf("Invalid choice\n");
            }
            switch(choice){
            case(1): // view available books
                sprintf(oper_request, "VAVBOOKS/0/x/x/0");
                break;
            case(2): // search by title
                memset(title,0,TITLE_SIZE);
                printf("Enter title: ");
                scanf(" %[^\n]",title);
                sprintf(oper_request,"SCHTITLE/0/%s/x/0",title);
                break;
            case(3): //search by author
                memset(author,0,AUTHOR_SIZE);
                printf("Enter author: ");
                scanf(" %[^\n]",title);
                sprintf(oper_request,"SCHAUTHOR/0/%s/x/0",author);
                break;
            case(4): // view borrowed books
                sprintf(oper_request,"MYBOOKS/0/x/x/0");
                break;
            case(5): // issue a book
                memset(title,0,TITLE_SIZE);
                printf("Enter title: ");
                scanf(" %[^\n]",title);
                sprintf(oper_request,"ISSUE/%d/%s/x/0",book_id,title);
                break;
            case(6): // return a book
                memset(title,0,TITLE_SIZE);
                printf("Enter title: ");
                scanf(" %[^\n]",title);
                sprintf(oper_request,"RETURN/%d/%s/x/0",book_id,title);
                break;
            case(7): // change password
                memset(oldp,0,CRED_SIZE);
                memset(newp,0,CRED_SIZE);
                printf("Enter old password: ");
                scanf(" %[^\n]",oldp);
                printf("Enter new password: ");
                scanf(" %[^\n]",newp);
                sprintf(oper_request,"UCHPWD/0/%s/%s/0",oldp,newp);
                break;
            case(8): // logout
                sprintf(oper_request,"ULOGOUT/0/x/x/0");
                break;
            default:
                break;
            }
            // send(*sock,oper_request,MSG_SIZE,0);
            //printf("%s\n",oper_request);
        }

        else{
            while(1){
                printf("Enter Choice: ");
                scanf("%d",&choice);
                if (choice>0 && choice<8) break;
                else printf("Invalid choice\n");
            }
            switch(choice){
            case(1): // add a book
                memset(title,0,TITLE_SIZE);
                memset(author,0,AUTHOR_SIZE);
                quantity = 0;
                printf("Enter name: ");
                scanf(" %[^\n]",title);
                printf("Enter author: ");
                scanf(" %[^\n]",author);
                printf("Enter quantity: ");
                scanf("%d",&quantity);
                sprintf(oper_request, "ADDBOOK/%d/%s/%s/%d",book_id,title,author,quantity);
                break;
            case(2): // remove a book
                printf("Enter book id: ");
                scanf("%d",&book_id);
                sprintf(oper_request,"RMBOOK/%d/x/x/0",book_id);
                break;
            case(3): // change book details
                memset(title,0,TITLE_SIZE);
                memset(author,0,AUTHOR_SIZE);
                quantity = 0;
                printf("Enter title: ");
                scanf(" %[^\n]",title);
                printf("Enter author: ");
                scanf(" %[^\n]",author);
                printf("Enter quantity: ");
                scanf("%d",&quantity);
                sprintf(oper_request, "CHBOOK/%d/%s/%s/%d",book_id,title,author,quantity);
                break;
            case(4): // remove a user
                memset(username,0,CRED_SIZE);
                printf("Enter username: ");
                scanf(" %[^\n]",username);
                sprintf(oper_request,"RMUSR/0/%s/x/0",username);
                break;
            case(5): // view all borrowed books
                sprintf(oper_request,"VBRBOOKS/0/x/x/0");
                break;
            case(6): // change password
                memset(oldp,0,CRED_SIZE);
                memset(newp,0,CRED_SIZE);
                printf("Enter old password: ");
                scanf(" %[^\n]",oldp);
                printf("Enter new password: ");
                scanf(" %[^\n]",newp);
                sprintf(oper_request,"ACHPWD/0/%s/%s/0",oldp,newp);
                break;
            case(7): // logout
                sprintf(oper_request,"ALOGOUT/0/x/x/0");
                break;
            default:
                break;
            }
            send(*sock,oper_request,MSG_SIZE,0);
            // printf("%s\n",oper_request);
        }
        // scanf(" %[^\n]",oper_request);
        // if (strcmp(oper_request, "logout") == 0){ // if message is logout, log out
        //     send(*sock, oper_request, MSG_SIZE, 0);
        //     printf("\nLogged Out.\n\n");
        //     break;
        // }
        // send(*sock, oper_request, MSG_SIZE, 0); // send message to server
        // read(*sock, oper_response, MSG_SIZE); // receive server response
        // printf("Server Response: %s\n\n", oper_response);
    }
    return;
}

int main(){
    int sock = 0;
    struct sockaddr_in server_address;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){ // IPv4 TCP-IP socket
        perror("SOCKET FAILED");
        return -1;
    }
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    if(inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr)<=0) { // Localhost
        perror("Invalid address/ Address not supported");
        return -1;
    }
    if (connect(sock, (struct sockaddr *)&server_address, sizeof(server_address)) < 0){
        perror("CONNECTION FAILED");
        return -1;
    }
    printf("Welcome to Online Library Management System\n\n");
    int is_admin = authenticate(&sock);
    operate(&sock, is_admin);
    close(sock);
    return 0;
}