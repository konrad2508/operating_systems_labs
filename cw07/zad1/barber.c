#include <stdio.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/sem.h>

#include "header.h"

struct queue* queuePtr = NULL;
int shmHandler = -1;
int semHandler = -1;

void sigtermHandler(){
    shmdt(queuePtr);
    shmctl(shmHandler, IPC_RMID, NULL);
    semctl(semHandler, 69, IPC_RMID);
    exit(0);
}

int main(int argc, char* argv[]) {

    if (argc < 2){
        printf("Too few arguments\n");
        return -1;
    }

    signal(SIGTERM, sigtermHandler);
    signal(SIGINT, sigtermHandler);

    key_t key = ftok(getenv("HOME"), 1);
    shmHandler = shmget(key, sizeof(struct queue), IPC_CREAT | 0755);
    semHandler = semget(key, 3, IPC_CREAT | 0755);

    queuePtr = (struct queue*) shmat(shmHandler, NULL, 0);
    initQueue(queuePtr, atoi(argv[1]));

    semctl(semHandler, CHAIR, SETVAL, 0);//is someone on the chair
    semctl(semHandler, QUEUE, SETVAL, 1);//to access shared queue

    while(1){

        //barber sleeps
        printf("[%ld] Barber sleeps.\n", timer());
        //wait until someone has taken the chair
        semaphoreTake(semHandler, CHAIR);

        //get pid of the dude in the chair

        semaphoreTake(semHandler, QUEUE);
        pid_t chairPid = queuePtr->chair;
        semaphoreGive(semHandler, QUEUE);

        printf("[%ld] %d woke barber.\n",timer(), chairPid);
        //cut him
        if (chairPid == -1){
            printf("Unknown error");
            exit(1);
        }

        printf("[%ld] Cutting %d.\n", timer(), chairPid);
        printf("[%ld] Finished cutting %d.\n", timer(), chairPid);
        kill(chairPid, SIGRTMIN+1);
        //free chair
        semaphoreTake(semHandler, QUEUE);
        leaveChair(queuePtr);
        semaphoreGive(semHandler, QUEUE);

        while(1){//cut everyone in the queue

            semaphoreTake(semHandler, QUEUE);
            pid_t toCut = pop(queuePtr);
            semaphoreGive(semHandler, QUEUE);

            if(toCut == -1){
                printf("[%ld] Queue empty.\n", timer());
                break;
            }
            else{
                printf("[%ld] %d got invited for cutting.\n", timer(), toCut);
                kill(toCut, SIGRTMIN);

                printf("[%ld] Cutting %d.\n", timer(), toCut);
                printf("[%ld] Finished cutting %d.\n", timer(), toCut);
                kill(toCut, SIGRTMIN+1);

            }

        }
    }

    return 0;
}
