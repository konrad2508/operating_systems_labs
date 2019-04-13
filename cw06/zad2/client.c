#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>
#include <signal.h>

#include <mqueue.h>

#include "header.h"

mqd_t clientHandle;
mqd_t serwerHandle;
char path[BUF_SIZE];

void close_queue(mqd_t queue){

    mq_close(queue);

}

void input(char *string, int length) {

    fgets(string,length,stdin);
    for(int x = 0; x <= length; x++) {

        if( string[x] == '\n') {
            string[x] = '\0';
            break;
        }

    }
}

void stop(int signum){

    char buf[BUF_SIZE];
    sprintf(buf, "%d:STOP", getpid());


    mq_send(serwerHandle, buf, strlen(buf) * sizeof(char), STOP);

    close_queue(clientHandle);
    close_queue(serwerHandle);
    mq_unlink(path);
    exit(0);
}

void end(int signum){
    close_queue(clientHandle);
    close_queue(serwerHandle);
    mq_unlink(path);
    printf("Server closed\n");
    exit(0);
}

void denied(int signum){
    close_queue(clientHandle);
    mq_unlink(path);
    printf("Connection denied: Too many clients\n");
    exit(0);
}

int main() {

    memset(path, 0, BUF_SIZE);
    sprintf(path, "/%d", getpid());

    struct mq_attr posixAttr;
    posixAttr.mq_maxmsg = MAX_MSG;
    posixAttr.mq_msgsize = BUF_SIZE;
    clientHandle = mq_open(path, O_RDONLY | O_CREAT | O_EXCL, 0755, &posixAttr);
    serwerHandle = mq_open("/server", O_WRONLY);

    if (clientHandle == -1){
        perror("Could not create client queue");
        return 1;
    }
    if (serwerHandle == -1){
        perror("Could not open server queue");
        return 1;
    }

    char buf[BUF_SIZE];
    sprintf(buf, "%d:NEW %s", getpid(), path);
    int a = mq_send(serwerHandle, buf, strlen(buf) * sizeof(char), NEW);
    if (a == -1) perror("mq_send() fail");
    signal(SIGINT, stop);
    signal(SIGRTMIN, end);
    signal(SIGRTMIN+1, denied);

    while(1){
        printf(">>");
        char command[BUF_SIZE];
        input(command, BUF_SIZE);
        if(command[0] == '\0' || command[0] == ' ') continue;
        char toSend[BUF_SIZE];
        enum msgtype priority;
        sprintf(toSend, "%d:%s", getpid(), command);

        char* cmd = strtok(command, " ");
        if (strcmp(cmd, "ADD") == 0){

            //checking arguments
            char* arg1 = strtok(NULL, " ");
            if (arg1 == NULL){
                printf("Too few arguments\n");
                continue;
            }
            char* arg2 = strtok(NULL, " ");
            if (arg2 == NULL) {
                printf("Too few arguments\n");
                continue;
            }

            priority = ADD;

        }else if (strcmp(cmd, "SUB") == 0){

            //checking arguments
            char* arg1 = strtok(NULL, " ");
            if (arg1 == NULL){
                printf("Too few arguments\n");
                continue;
            }
            char* arg2 = strtok(NULL, " ");
            if (arg2 == NULL) {
                printf("Too few arguments\n");
                continue;
            }

            priority = SUB;

        }else if (strcmp(cmd, "DIV") == 0){

            //checking arguments
            char* arg1 = strtok(NULL, " ");
            if (arg1 == NULL){
                printf("Too few arguments\n");
                continue;
            }
            char* arg2 = strtok(NULL, " ");
            if (arg2 == NULL) {
                printf("Too few arguments\n");
                continue;
            }

            priority = DIV;

        }else if (strcmp(cmd, "MUL") == 0){

            //checking arguments
            char* arg1 = strtok(NULL, " ");
            if (arg1 == NULL){
                printf("Too few arguments\n");
                continue;
            }
            char* arg2 = strtok(NULL, " ");
            if (arg2 == NULL) {
                printf("Too few arguments\n");
                continue;
            }

            priority = MUL;

        }else if (strcmp(cmd, "TIME") == 0){

            priority = TIME;

        }else if (strcmp(cmd, "MIRROR") == 0){

            //checking arguments
            char* arg1 = strtok(NULL, " ");
            if (arg1 == NULL){
                printf("Too few arguments\n");
                continue;
            }

            priority = MIRROR;

        }else if (strcmp(cmd, "STOP") == 0){

            priority = STOP;

        }else if (strcmp(cmd, "END") == 0){

            priority = END;

        }else{
            printf("Unknown command\n");
            continue;
        }

        mq_send(serwerHandle, toSend, strlen(toSend) * sizeof(char), priority);

        struct mq_attr info;
        mq_getattr(clientHandle, &info);

        if (strcmp(cmd, "STOP") == 0 || strcmp(cmd, "END") == 0){
            break;
        }

        while(info.mq_curmsgs == 0){

            mq_getattr(clientHandle, &info);

        }

        char msg[BUF_SIZE];
        memset(msg,0,BUF_SIZE);
        mq_receive(clientHandle, msg, BUF_SIZE, NULL);

        printf("%s\n", msg);
    }

    close_queue(clientHandle);
    mq_unlink(path);

    return 0;
}
