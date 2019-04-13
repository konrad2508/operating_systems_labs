#include <stdio.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <memory.h>
#include <stdlib.h>
#include <unistd.h>

#include "header.h"

int clientQ[MAX_CLIENTS];
pid_t clientId[MAX_CLIENTS];
int qHandle;
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

int create_queue(){

    key_t klucz = ftok(getenv("HOME"), SERVER_ID);
    printf("Key: %d\n", klucz);
    return msgget(klucz, IPC_CREAT | 0755);

}

int open_queue(key_t klucz){
    return msgget(klucz, 0);
}

void close_queue(int queue){

    msgctl(queue, IPC_RMID, NULL);

}

int find_queue(pid_t pid){
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
            kill(clientId[i], SIGRTMIN);
        }
    }

    close_queue(qHandle);
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

    struct msqid_ds info;
    while(1){

        //message form:
        //pid:command <arg>

        msgctl(qHandle, IPC_STAT, &info);
        if (info.msg_qnum > 0){
            //com exists
            struct msgbuf msgrec;
            msgrcv(qHandle, &msgrec, BUF_SIZE, 0, 0);

            printf("%s\n", msgrec.mtext);

            pid_t id = atoi(strtok(msgrec.mtext, ":"));
            char* cmd = strtok(NULL, ":");

            char* cmdType = strtok(cmd, " ");

            //resolving cmd type
            if (strcmp(cmdType, "NEW") == 0){
                //new client
                if (currentClients >= MAX_CLIENTS){
                    kill(id, SIGRTMIN+1);
                }else {
                    currentClients++;
                    key_t qId = atoi(strtok(NULL, " "));
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
                struct msgbuf toSend;
                toSend.mtype = MIRROR;
                strcpy(toSend.mtext, mirrored);

                int clientHandle = find_queue(id);
                if (clientHandle == -1){
                    printf("Queue not found\n");
                }else{
                    msgsnd(clientHandle, &toSend, sizeof(long) + (strlen(toSend.mtext)*sizeof(char)), 0);
                }

            }
            else if (strcmp(cmdType, "ADD") == 0){
                int num1 = atoi(strtok(NULL, " "));
                int num2 = atoi(strtok(NULL, " "));
                int res = num1 + num2;
                struct msgbuf toSend;
                toSend.mtype = ADD;
                sprintf(toSend.mtext, "%d", res);
                int clientHandle = find_queue(id);
                if (clientHandle == -1){
                    printf("Queue not found\n");
                }else{
                    msgsnd(clientHandle, &toSend, sizeof(long) + (strlen(toSend.mtext)*sizeof(char)), 0);
                }
            }
            else if (strcmp(cmdType, "MUL") == 0){
                int num1 = atoi(strtok(NULL, " "));
                int num2 = atoi(strtok(NULL, " "));
                int res = num1 * num2;

                struct msgbuf toSend;
                toSend.mtype = MUL;
                sprintf(toSend.mtext, "%d", res);

                int clientHandle = find_queue(id);
                if (clientHandle == -1){
                    printf("Queue not found\n");
                }else{
                    msgsnd(clientHandle, &toSend, sizeof(long) + (strlen(toSend.mtext)*sizeof(char)), 0);
                }
            }
            else if (strcmp(cmdType, "SUB") == 0){
                int num1 = atoi(strtok(NULL, " "));
                int num2 = atoi(strtok(NULL, " "));
                int res = num1 - num2;

                struct msgbuf toSend;
                toSend.mtype = SUB;
                sprintf(toSend.mtext, "%d", res);

                int clientHandle = find_queue(id);
                if (clientHandle == -1){
                    printf("Queue not found\n");
                }else{
                    msgsnd(clientHandle, &toSend, sizeof(long) + (strlen(toSend.mtext)*sizeof(char)), 0);
                }

            }
            else if (strcmp(cmdType, "DIV") == 0){
                int num1 = atoi(strtok(NULL, " "));
                int num2 = atoi(strtok(NULL, " "));
                int res = num1 / num2;

                struct msgbuf toSend;
                toSend.mtype = DIV;
                sprintf(toSend.mtext, "%d", res);

                int clientHandle = find_queue(id);
                if (clientHandle == -1){
                    printf("Queue not found\n");
                }else{
                    msgsnd(clientHandle, &toSend, sizeof(long) + (strlen(toSend.mtext)*sizeof(char)), 0);
                }

            }
            else if (strcmp(cmdType, "TIME") == 0){
                FILE* date = popen("date", "r");

                struct msgbuf toSend;
                toSend.mtype = TIME;
                fgets(toSend.mtext, BUF_SIZE, date);

                int clientHandle = find_queue(id);
                if (clientHandle == -1){
                    printf("Queue not found\n");
                }else{
                    msgsnd(clientHandle, &toSend, sizeof(long) + (strlen(toSend.mtext)*sizeof(char)), 0);
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