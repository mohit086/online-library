#include "../headers/head.h"
int main(){
    sem_t s1, s2, s3;
    sem_init(&s1, 0, 1); sem_init(&s2, 0, 1); sem_init(&s3, 0, 1);
    sem_wait(&s1); sem_wait(&s2); sem_wait(&s3);
    int fd1 = open("../db/book.dat",O_CREAT | O_WRONLY, 0666);
    int fd2 = open("../db/user.dat",O_CREAT | O_WRONLY, 0666);
    int fd3 = open("../db/issue.dat",O_CREAT | O_WRONLY, 0666);
    ftruncate(fd1, 0); ftruncate(fd2, 0); ftruncate(fd3, 0);
    close(fd1); close(fd2); close(fd3);
    printf("Success\n");
    sem_post(&s3); sem_post(&s2); sem_post(&s1);
}