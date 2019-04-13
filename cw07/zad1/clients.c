#include <stdio.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <sys/wait.h>
#include <asm/errno.h>
#include <errno.h>

#include "header.h"

int isCut = 0;
int isSeated = 0;
int cutTimes = 0;

struct queue* queuePtr = NULL;
int shmHandler = -1;
int semHandler = -1;

void seated(){
    
    isSeated = 1;

}

void cut(){

    isCut = 1;

}

int main(int argc, char* argv[]) {

    if (argc < 3){
        printf("Too few arguments\n");
        return 1;
    }

    signal(SIGRTMIN, seated);
    signal(SIGRTMIN+1, cut);

    key_t key = ftok(getenv("HOME"), 1);
    shmHandler = shmget(key, 0, 0);
    semHandler = semget(key, 0, 0);
    queuePtr = (struct queue*) shmat(shmHandler, NULL, 0);

    int clientsN = atoi(argv[1]);
    int cutN = atoi(argv[2]);

    while(clientsN > 0){
        pid_t clientId = fork();

        if (clientId == 0){

            while(cutN > 0){
                //access shared memory
                semaphoreTake(semHandler, QUEUE);

                if (isEmpty(queuePtr)){ //if no one is waiting

                    // check if someone is being cut
                    if (queuePtr->chair != -1){ //someone is on the chair
                        push(queuePtr, getpid()); // self add to queue
                        printf("[%ld] %d sat in the queue.\n", timer(), getpid());
                    }
                    else{
                        //else client sat on the chair
                        sit(queuePtr, getpid());
                        printf("[%ld] %d woke the barber.\n", timer(), getpid());
                        semaphoreGive(semHandler, CHAIR);
                        isSeated = 1;
                    }
                    
                    //stop accessing shared memory
                    semaphoreGive(semHandler, QUEUE);

                    while(!isSeated){}
                    printf("[%ld] %d got seated.\n", timer(), getpid());
                    isSeated = 0;
                    while(!isCut){}
                    printf("[%ld] %d got cut.\n", timer(), getpid());
                    isCut = 0;
                }else{ //if someone is waiting
                    if (!isFull(queuePtr)){ // if there is a free chair in the lobby
                        push(queuePtr, getpid());
                        printf("[%ld] %d sat in the queue.\n",timer(), getpid());
                        //stop accessing shared memory
                        semaphoreGive(semHandler, QUEUE);

                        while(!isSeated){}
                        printf("[%ld] %d got seated.\n", timer(), getpid());
                        isSeated = 0;
                        while(!isCut){}
                        printf("[%ld] %d got cut.\n", timer(), getpid());
                        isCut = 0;
                    }else{ //else client has to leave the barbershop

                        //stop accessing shared memory
                        semaphoreGive(semHandler, QUEUE);

                        printf("[%ld] No free chair for %d.\n", timer(), getpid());

                        //was not cut so he has to come back one more time
                        cutN++;
                    }
                }
                printf("[%ld] %d leaves the shop.\n", timer(), getpid());
                cutN--;
            }
            printf("[%ld] %d dies.\n",timer(), getpid());
            exit(0); // was cut N times so dies
        }

        clientsN--;
    }

    while(1){
        wait(NULL);

        if (errno == ECHILD){
            break;
        }
    }

    return 0;
}
