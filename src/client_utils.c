#include "../headers/head.h"
#include "../headers/client_utils.h"

int authenticate(int* sock){
    char username[CRED_SIZE], password[CRED_SIZE], request[MSG_SIZE], response[MSG_SIZE];
    int choice, is_authenticated = 0, is_admin = 0;
    while(!is_authenticated){
        memset(username, 0, CRED_SIZE);
        memset(password, 0, CRED_SIZE);
        memset(response, 0, MSG_SIZE);
        while(1){
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
        sprintf(request, "AUTH %d %s/%s", choice, username, password);
        write(*sock, request, MSG_SIZE);
        read(*sock, response, MSG_SIZE);
        if(strcmp(response,"AUTH_SUCCESS")==0 || strcmp(response,"ADMIN_AUTH_SUCCESS")==0){
            is_authenticated = 1;
            printf("\nAuthenticated successfully as %s\n\n",username);
            if (strcmp(response,"ADMIN_AUTH_SUCCESS")==0 || strcmp(username,"admin")==0) is_admin = 1;
        }
        else printf("AUTHENTICATION ERROR - %s\n\n",response);
    }
    return is_admin;
}

void operate(int* sock, int is_admin){
    char request[MSG_SIZE], response[MSG_SIZE];
    char title[CRED_SIZE], author[CRED_SIZE], username[CRED_SIZE], passw[CRED_SIZE];
    int choice, quantity, book_id;

    if (!is_admin){
        printf("Choose any option number: \n");
        printf("1 - View available books\n");
        printf("2 - View my issued books\n");
        printf("3 - Issue a book\n");
        printf("4 - Return a book\n");
        printf("5 - Logout\n\n");
    }
    else {
        printf("Choose any option number: \n");
        printf("1 - Add a book\n");
        printf("2 - Remove a book\n");
        printf("3 - Change book quantity\n");
        printf("4 - Remove a user\n");
        printf("5 - View all available books\n");
        printf("6 - View all issued books\n");
        printf("7 - Add admin\n");
        printf("8 - Logout\n\n");
    }
    while(1){
        memset(response, '\0', MSG_SIZE);
        if (!is_admin){
            while(1){
                printf("Enter choice: ");
                if (scanf("%d",&choice)!= 1 || (choice<1 || choice>8)){
                    memset(response, '\0', MSG_SIZE);
                    while (getchar() != '\n');
                    continue;
                }
                break;
            }
            switch(choice){
            case(1): // view available books
                sprintf(request, "VAVBKS/0/x/x/0");
                break;
            case(2): // view issued books
                sprintf(request,"MYBOOK/0/x/x/0");
                break;
            case(3): // issue a book
                memset(title,0,TITLE_SIZE);
                printf("Enter id: ");
                scanf("%d",&book_id);
                sprintf(request,"ISSUBK/%d/x/x/0",book_id);
                break;
            case(4): // return a book
                memset(title,0,TITLE_SIZE);
                printf("Enter id: ");
                scanf("%d",&book_id);
                sprintf(request,"RTRNBK/%d/x/x/0",book_id);
                break;
            case(5): // logout
                sprintf(request,"LOGOUT/0/x/x/0");
                break;
            default:
                break;
            }
        }

        else{
            while(1){
                printf("Enter choice: ");
                if (scanf("%d",&choice)!= 1 || (choice<1 && choice>8)){
                    memset(response, '\0', MSG_SIZE);
                    while (getchar() != '\n');
                    continue;
                }
                break;
            }
            switch(choice){
            case(1): // add a book
                memset(title,0,TITLE_SIZE);
                memset(author,0,AUTHOR_SIZE);
                printf("Enter id: ");
                scanf("%d",&book_id);
                printf("Enter name: ");
                scanf(" %[^\n]",title);
                printf("Enter author: ");
                scanf(" %[^\n]",author);
                printf("Enter quantity: ");
                scanf("%d",&quantity);
                sprintf(request, "ADBOOK/%d/%s/%s/%d",book_id,title,author,quantity);
                break;
            case(2): // remove a book
                printf("Enter book id: ");
                scanf("%d",&book_id);
                sprintf(request,"RMBOOK/%d/x/x/0",book_id);
                break;
            case(3): // change book quantity
                printf("Enter id: ");
                scanf("%d", &book_id);
                printf("Enter new quantity: ");
                scanf("%d",&quantity);
                sprintf(request, "CHBQTY/%d/x/x/%d",book_id,quantity);
                break;
            case(4): // remove a user
                memset(username,0,CRED_SIZE);
                printf("Enter username: ");
                scanf(" %[^\n]",username);
                sprintf(request,"RMUSER/0/%s/x/0",username);
                break;
            case(5): // view available books
                sprintf(request, "VAVBKS/0/x/x/0");
                break;
            case(6): // view all issued books
                sprintf(request,"VISUBS/0/x/x/0");
                break;
            case(7): // add another admin
                memset(username,0,CRED_SIZE);
                memset(passw,0,CRED_SIZE);
                printf("Enter username: ");
                scanf(" %[^\n]",username);
                printf("Enter password: ");
                scanf(" %[^\n]",passw);
                sprintf(request,"AADMIN/0/%s/%s/0",username,passw);
                break;
            case(8): // logout
                sprintf(request,"LOGOUT/0/x/x/0");
                break;
            default:
                break;
            }
        }
        write(*sock, request, MSG_SIZE);
        if (strcmp(request, "LOGOUT/0/x/x/0") == 0){
            printf("\nLOGGED OUT\n\n");
            break;
        }
        read(*sock, response, MSG_SIZE);
        printf("%s", response);
    }
    return;
}

void ignore_sigint(){
    ;
}