#include "../headers/head.h"
#include "../headers/server_utils.h"

sem_t book_sem;
sem_t user_sem;
sem_t issue_sem;
int get_client(char *username){
    for (int i = 0; i < MAX_CLIENTS; i++) if (strcmp(online_arr[i].name, username) == 0) return i;
    return -1;
}

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

void server_side_authenticate(int* sock, char* auth_request, char* auth_response, User* user){
    sem_wait(&user_sem);
    while (1){
        char username[CRED_SIZE];
        char password[CRED_SIZE];
        int choice, username_exists = 0;

        if (read(*sock, auth_request, MSG_SIZE) <= 0) return;
        sscanf(auth_request, "AUTH %d %[^/]/%s", &choice, username, password);

        int fd = open("../db/user.dat", O_RDONLY);
        if (fd == -1){
            perror("DATABASE_ERROR");
            sem_post(&user_sem);
            return;
        };
        User* temp = (User*)malloc(sizeof(User));
        while(read(fd,temp,sizeof(User))){
            if (!strcmp(temp->username,username)){
                username_exists = 1;
                break;
            }
        }
        close(fd);

        if (choice == 0){
            if (username_exists) strcpy(auth_response, "USERNAME EXISTS");
            else {
                fd = open("../db/user.dat", O_RDWR|O_APPEND, 0666);
                strcpy(user->username,username);
                strcpy(user->password,password);
                user->is_admin = 0;
                user->valid = 1;
                write(fd,user,sizeof(User));
                close(fd);
                add_user(username);
                strcpy(auth_response, "AUTH_SUCCESS");
            }
        }

        else if (choice == 1){
            if (username_exists && temp->valid == 1){
                if (strcmp(password, temp->password) == 0){
                    if (online_arr[get_client(username)].is_online) strcpy(auth_response, "ALREADY LOGGED IN");
                    else {
                        if (temp->is_admin) strcpy(auth_response, "ADMIN_AUTH_SUCCESS");
                        else strcpy(auth_response, "AUTH_SUCCESS");
                        add_user(username);
                        strcpy(user->username,temp->username);
                        strcpy(user->password,temp->password);
                    }
                }
                else strcpy(auth_response, "INVALID PASSWORD");
            }
            else strcpy(auth_response, "USERNAME NOT FOUND");
        }  
        write(*sock, auth_response, MSG_SIZE);
        if (strcmp(auth_response, "AUTH_SUCCESS") == 0){
            printf("%s CONNECTED\n\n", username);
            break;
        }
    }
    sem_post(&user_sem);
}

void add_book(int id, char* title, char* author, int quantity, char* response){
    sem_wait(&book_sem);
    memset(response,'\0',MSG_SIZE);
    Book* book = (Book*)malloc(sizeof(Book));
    int fd = open("../db/book.dat", O_RDWR | O_APPEND, 0666);
    while(read(fd,book,sizeof(Book))){
        if(book->valid && (book->id ==id || strcmp(book->title,title)==0)){
            sprintf(response,"ID OR TITLE MATCHES EXISTING BOOK");
            sem_post(&book_sem);
            return;
        }
    }
    book->id = id;
    strcpy(book->title,title);
    strcpy(book->author,author);
    book->quantity = quantity;
    book->valid = 1;
    write(fd,book,sizeof(Book));
    close(fd);
    sprintf(response,"BOOK %d ADDED SUCCESSFULLY",id);
    sem_post(&book_sem);
}

void remove_book(int id, char* response){
    sem_wait(&book_sem);
    memset(response,'\0',MSG_SIZE);
    Book* book = (Book*)malloc(sizeof(Book));
    int fd = open("../db/book.dat", O_RDWR, 0666);
    while(read(fd,book,sizeof(Book))){
        if (book->id == id){
            book->valid = 0;
            lseek(fd, -sizeof(Book), SEEK_CUR);
            write(fd,book,sizeof(Book));
            close(fd);
            sprintf(response,"BOOK %d REMOVED SUCCESSFULLY",id);
            sem_post(&user_sem);
            return;
        }
    }
    sprintf(response,"BOOK %d NOT FOUND\n",id);
    close(fd);
    sem_post(&book_sem);
}

void change_qty(int id, int quantity, char* response){
    sem_wait(&book_sem);
    memset(response,'\0',MSG_SIZE);
    Book* book = (Book*)malloc(sizeof(Book));
    int fd = open("../db/book.dat", O_RDWR, 0666);
    while(read(fd,book,sizeof(Book))){
        if (book->id == id && book->valid==1){
            book->quantity = quantity;
            lseek(fd, -sizeof(Book), SEEK_CUR);
            write(fd,book,sizeof(Book));
            close(fd);
            sprintf(response,"UPDATED QUANTITY FOR BOOK %d\n",id);
            sem_post(&book_sem);
            return;
        }
    }
    sprintf(response,"BOOK %d NOT FOUND\n",id);
    close(fd);
    sem_post(&book_sem);
}

void remove_user(char* username, char* response){
    sem_wait(&user_sem);
    memset(response,'\0',MSG_SIZE);
    User* user = (User*)malloc(sizeof(User));
    int fd = open("../db/user.dat", O_RDWR, 0666);
    while(read(fd,user,sizeof(User))){
        if (strcmp(user->username, username)==0){
            user->valid = 0;
            lseek(fd, -sizeof(User), SEEK_CUR);
            write(fd,user,sizeof(User));
            close(fd);
            sprintf(response,"USER %s REMOVED SUCESSFULLY",username);
            sem_post(&user_sem);
            return;
        }
    }
    sprintf(response,"USER %s NOT FOUND",username);
    close(fd);
    sem_post(&user_sem);
}

void view_all_issues(char* response){
    sem_wait(&issue_sem);
    memset(response,'\0',MSG_SIZE);
    char line[MSG_SIZE];
    Issue* issue = (Issue*)malloc(sizeof(Issue));
    int fd = open("../db/issue.dat", O_RDONLY);
    while(read(fd, issue,sizeof(Issue))){
        if (issue->valid){
            sprintf(line,"USER %s HAS ISSUED BOOK %d\n", issue->user, issue->book_id);
            strcat(response,line);
        }
    }
    close(fd);
    sem_post(&issue_sem);
}

void view_avl_books(char* response){
    sem_wait(&book_sem);
    memset(response,'\0',MSG_SIZE);
    char line[MSG_SIZE];
    Book* book = (Book*)malloc(sizeof(Book));
    int fd = open("../db/book.dat", O_RDONLY);
    while(read(fd,book,sizeof(Book))){
        if(book->valid){
            sprintf(line,"ID: %d\nTITLE: %s\nAUTHOR: %s\nCOPIES: %d\n\n", book->id, book->title, book->author, book->quantity);
            strcat(response,line);
        }
    }
    close(fd);
    sem_post(&book_sem);
}

void view_mybooks(char* username, char* response){
    sem_wait(&issue_sem);
    memset(response,'\0',MSG_SIZE);
    char line[MSG_SIZE];
    Issue* issue = (Issue*)malloc(sizeof(Issue));
    int fd = open("../db/issue.dat", O_RDONLY);
    while(read(fd, issue,sizeof(Issue))){
        if (strcmp(issue->user,username)==0 && issue->valid){
            sprintf(line,"YOU HAVE ISSUED BOOK %d\n", issue->book_id);
            strcat(response,line);
        }
    }
    close(fd);
    sem_post(&issue_sem);
}

void issue_book(char* username, int id, char* response){
    sem_wait(&book_sem);
    sem_wait(&issue_sem);
    memset(response,'\0',MSG_SIZE);
    Book* book = (Book*)malloc(sizeof(Book));
    int fd = open("../db/book.dat",O_RDWR);
    while(read(fd,book,sizeof(Book))){
        if (book->id == id && book->valid == 1){

            Issue* issue = (Issue*)malloc(sizeof(Issue));
            int fd2 = open("../db/issue.dat",O_RDWR|O_APPEND, 0666);
            while(read(fd2,issue,sizeof(Issue))){
                if(issue->book_id == id && strcmp(issue->user,username)==0 && issue->valid){
                    sprintf(response,"BOOK ALREADY ISSUED");
                    sem_post(&issue_sem);
                    sem_post(&book_sem);
                    return;
                }
            }
            strcpy(issue->user,username);
            issue->book_id = id;
            issue->valid = 1;
            write(fd2,issue,sizeof(Issue));
            close(fd2);

            book->quantity--;
            if(book->quantity==0) book->valid = 0;
            lseek(fd,-sizeof(Book),SEEK_CUR);
            write(fd,book,sizeof(Book));
            close(fd);

            sprintf(response,"BOOK %d ISSUED SUCCESSFULLY",id);
            sem_post(&issue_sem);
            sem_post(&book_sem);
            return;
        }
    }
    sprintf(response,"BOOK NOT FOUND");
    sem_post(&issue_sem);
    sem_post(&book_sem);
}

void return_book(char* username, int id, char* response){
    sem_wait(&issue_sem);
    sem_wait(&book_sem);
    memset(response,'\0',MSG_SIZE);
    Issue* issue = (Issue*)malloc(sizeof(Issue));
    int fd = open("../db/issue.dat",O_RDWR);
    while(read(fd,issue,sizeof(Issue))){
        if (strcmp(issue->user, username)==0 && issue->book_id==id && issue->valid){
            issue->valid = 0;
            lseek(fd,-sizeof(Issue),SEEK_CUR);
            write(fd,issue,sizeof(Issue));
            close(fd);

            Book* book = (Book*)malloc(sizeof(Book));
            int fd2 = open("../db/book.dat",O_RDWR);
            while(read(fd2,book,sizeof(Book))){
                if(book->id==id && book->valid){
                    book->quantity++;
                    lseek(fd2,-sizeof(Book),SEEK_CUR);
                    write(fd2,book,sizeof(Book));
                    close(fd2);
                    sprintf(response,"BOOK %d RETURNED SUCCESSFULY",id);
                    sem_post(&book_sem);
                    sem_post(&issue_sem);
                    return;
                }
            }
        }
    }
    sprintf(response,"ERROR IN RETURNING BOOK");
    sem_post(&book_sem);
    sem_post(&issue_sem);
}

void add_admin(char* username,char* password, char* response){
    sem_wait(&user_sem);
    User* user = (User*)malloc(sizeof(User));
    int fd = open("../db/user.dat",O_RDWR|O_APPEND);
    while(read(fd,user,sizeof(User))){
        if(strcmp(user->username,username)==0){
            sprintf(response,"ADMIN ADD ERROR - USERNAME EXISTS");
            close(fd);
            return;
        }
    }
    strcpy(user->username,username);
    strcpy(user->password,password);
    user->is_admin = 1;
    user->valid = 1;
    write(fd,user,sizeof(User));
    close(fd);
    sprintf(response,"ADMIN ADDED SUCCESSFULLY");
    sem_post(&user_sem);
}