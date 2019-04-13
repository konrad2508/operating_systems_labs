#include <stdio.h>
#include <sys/param.h>
#include <sys/types.h>
#include <memory.h>
#include <stdlib.h>
#include <unistd.h>

#include <mqueue.h>

#include "header.h"

mqd_t clientQ[MAX_CLIENTS];
pid_t clientId[MAX_CLIENTS];
mqd_t qHandle;
int currentClients;

char* strrev(char* str) {
    int i = strlen(str) - 1;
    int j = 0;
    char ch;

    while (i > j) {
        ch = str[i];
        str[i] = str[j];
        str[j] = ch;
        i--;
        j++;
    }

    return str;
}

mqd_t create_queue(){

    struct mq_attr posixAttr;
    posixAttr.mq_maxmsg = MAX_MSG;
    posixAttr.mq_msgsize = BUF_SIZE;
    return mq_open("/server", O_RDONLY | O_CREAT | O_EXCL, 0755, &posixAttr);

}

mqd_t open_queue(char* path){

    return mq_open(path, O_WRONLY);

}

void close_queue(mqd_t queue){

    mq_close(queue);

}

mqd_t find_queue(pid_t pid){
    for (int i = 0; i < MAX_CLIENTS; i++){
        if (clientId[i] == pid)
            return clientQ[i];
    }
    return -1;
}

int find_free_id(){
    for (int i = 0; i < MAX_CLIENTS; i++){
        if (clientId[i] == 0)
            return i;
    }
    return -1;
}

int find_id(pid_t pid){
    for (int i = 0; i < MAX_CLIENTS; i++){
        if (clientId[i] == pid)
            return i;
    }
    return -1;
}

void stop(int signum){

    for (int i = 0; i < MAX_CLIENTS; i++){
        if (clientId[i] != 0){
            close_queue(clientQ[i]);
            kill(clientId[i], SIGRTMIN);
        }
    }

    close_queue(qHandle);
    mq_unlink("/server");
    exit(0);
}

int main() {

    qHandle = create_queue();
    currentClients = 0;

    if (qHandle == -1){
        perror("Failed to create server IPC");
        return 1;
    }else{
        printf("Server handle: %d\n", qHandle);
    }

    for (int i = 0; i < MAX_CLIENTS; i++){
        clientQ[i] = 0;
        clientId[i] = 0;
    }

    signal(SIGINT, stop);

    struct mq_attr info;
    while(1){

        //message form:
        //pid:command <arg>

        mq_getattr(qHandle, &info);
        if (info.mq_curmsgs > 0){
            //com exists
            char msg[BUF_SIZE];
            memset(msg,0,BUF_SIZE);
            int a = mq_receive(qHandle, msg, BUF_SIZE, NULL);
            if (a == -1) perror("mq_receive() fail");
            printf("%s\n", msg);
            pid_t id = atoi(strtok(msg, ":"));
            char* cmd = strtok(NULL, ":");

            char* cmdType = strtok(cmd, " ");

            //resolving cmd type
            if (strcmp(cmdType, "NEW") == 0){
                //new client
                if (currentClients >= MAX_CLIENTS){
                    kill(id, SIGRTMIN+1);
                }else {
                    currentClients++;
                    char* qId = strtok(NULL, " ");
                    int index = find_free_id();

                    clientQ[index] = open_queue(qId);
                    clientId[index] = id;

                    printf("*****%d connected*****\n", id);
                }

            }
            else if (strcmp(cmdType, "MIRROR") == 0){
                char* toMirror = strtok(NULL, " ");
                char buf[BUF_SIZE];
                strcpy(buf, toMirror);
                char* mirrored = strrev(buf);

                mqd_t clientHandle = find_queue(id);
                if (clientHandle == -1){
                    printf("Queue not found\n");
                }else{
                    mq_send(clientHandle, mirrored, strlen(mirrored) * sizeof(char), MIRROR);
                }

            }
            else if (strcmp(cmdType, "ADD") == 0){
                int num1 = atoi(strtok(NULL, " "));
                int num2 = atoi(strtok(NULL, " "));
                int res = num1 + num2;
                char buf[BUF_SIZE];
                sprintf(buf, "%d", res);
                mqd_t clientHandle = find_queue(id);
                if (clientHandle == -1){
                    printf("Queue not found\n");
                }else{
                    mq_send(clientHandle, buf, strlen(buf) * sizeof(char), ADD);
                }
            }
            else if (strcmp(cmdType, "MUL") == 0){
                int num1 = atoi(strtok(NULL, " "));
                int num2 = atoi(strtok(NULL, " "));
                int res = num1 * num2;

                char buf[BUF_SIZE];
                sprintf(buf, "%d", res);
                mqd_t clientHandle = find_queue(id);
                if (clientHandle == -1){
                    printf("Queue not found\n");
                }else{
                    mq_send(clientHandle, buf, strlen(buf) * sizeof(char), MUL);
                }
            }
            else if (strcmp(cmdType, "SUB") == 0){
                int num1 = atoi(strtok(NULL, " "));
                int num2 = atoi(strtok(NULL, " "));
                int res = num1 - num2;

                char buf[BUF_SIZE];
                sprintf(buf, "%d", res);
                mqd_t clientHandle = find_queue(id);
                if (clientHandle == -1){
                    printf("Queue not found\n");
                }else{
                    mq_send(clientHandle, buf, strlen(buf) * sizeof(char), SUB);
                }

            }
            else if (strcmp(cmdType, "DIV") == 0){
                int num1 = atoi(strtok(NULL, " "));
                int num2 = atoi(strtok(NULL, " "));
                int res = num1 / num2;

                char buf[BUF_SIZE];
                sprintf(buf, "%d", res);
                mqd_t clientHandle = find_queue(id);
                if (clientHandle == -1){
                    printf("Queue not found\n");
                }else{
                    mq_send(clientHandle, buf, strlen(buf) * sizeof(char), DIV);
                }

            }
            else if (strcmp(cmdType, "TIME") == 0){
                FILE* date = popen("date", "r");

                char buf[BUF_SIZE];
                fgets(buf, BUF_SIZE, date);

                mqd_t clientHandle = find_queue(id);
                if (clientHandle == -1){
                    printf("Queue not found\n");
                }else{
                    mq_send(clientHandle, buf, strlen(buf) * sizeof(char), TIME);
                }

                pclose(date);
            }
            else if (strcmp(cmdType, "STOP") == 0){
                int index = find_id(id);

                close_queue(clientQ[index]);
                currentClients--;

                clientId[index] = 0;
                clientQ[index] = 0;

                printf("*****%d disconnected*****\n", id);
            }
            else if (strcmp(cmdType, "END") == 0){

                raise(SIGINT);

            }
        }

        //sleep(1);
    }

    return 0;
}