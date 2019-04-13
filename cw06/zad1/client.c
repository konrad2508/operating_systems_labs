#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>
#include <signal.h>

#include "header.h"

int clientHandle;
int serwerHandle;

void close_queue(int queue){

    msgctl(queue, IPC_RMID, NULL);

}

void input(char *string,int length) {

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

    struct msgbuf toSend;
    toSend.mtype = 1;
    strcpy(toSend.mtext, buf);
    msgsnd(serwerHandle, &toSend, sizeof(long) + (strlen(toSend.mtext)*sizeof(char)), 0);

    close_queue(clientHandle);
    exit(0);
}

void end(int signum){
    close_queue(clientHandle);
    printf("Server closed\n");
    exit(0);
}

void denied(int signum){
    close_queue(clientHandle);
    printf("Connection denied: Too many clients\n");
    exit(0);
}

int main() {

    key_t clientKey = ftok(getenv("HOME"), getpid());

    clientHandle = msgget(clientKey, IPC_CREAT | IPC_EXCL | 0755);
    serwerHandle = msgget(ftok(getenv("HOME"), SERVER_ID), IPC_CREAT | 0755);

    if (clientHandle == -1){
        perror("Could not create client queue");
        return 1;
    }
    if (serwerHandle == -1){
        perror("Could not open server queue");
        return 1;
    }

    char buf[BUF_SIZE];
    sprintf(buf, "%d:NEW %d", getpid(), clientKey);

    struct msgbuf toSend;
    toSend.mtype = NEW;
    strcpy(toSend.mtext, buf);

    msgsnd(serwerHandle, &toSend, sizeof(long) + (strlen(toSend.mtext)*sizeof(char)), 0);

    signal(SIGINT, stop);
    signal(SIGRTMIN, end);
    signal(SIGRTMIN+1, denied);

    while(1){
        printf(">>");
        char command[BUF_SIZE];
        input(command, BUF_SIZE);
        if(command[0] == '\0' || command[0] == ' ') continue;
        char b[BUF_SIZE];
        sprintf(b, "%d:%s", getpid(), command);

        char* cmd = strtok(command, " ");
        strcpy(toSend.mtext, b);
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

            toSend.mtype = ADD;

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

            toSend.mtype = SUB;

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

            toSend.mtype = DIV;

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

            toSend.mtype = MUL;

        }else if (strcmp(cmd, "TIME") == 0){

            toSend.mtype = TIME;

        }else if (strcmp(cmd, "MIRROR") == 0){

            //checking arguments
            char* arg1 = strtok(NULL, " ");
            if (arg1 == NULL){
                printf("Too few arguments\n");
                continue;
            }

            toSend.mtype = MIRROR;

        }else if (strcmp(cmd, "STOP") == 0){

            toSend.mtype = STOP;

        }else if (strcmp(cmd, "END") == 0){

            toSend.mtype = END;

        }else{
            printf("Unknown command\n");
            continue;
        }

        msgsnd(serwerHandle, &toSend, sizeof(long) + (strlen(toSend.mtext)*sizeof(char)), 0);

        struct msqid_ds info;
        msgctl(clientHandle, IPC_STAT, &info);

        if (strcmp(cmd, "STOP") == 0 || strcmp(cmd, "END") == 0){
            break;
        }

        while(info.msg_qnum == 0){

            msgctl(clientHandle, IPC_STAT, &info);

        }

        struct msgbuf msg;
        msgrcv(clientHandle, &msg, BUF_SIZE, 0, 0);

        printf("%s\n", msg.mtext);
    }

    close_queue(clientHandle);

    return 0;
}