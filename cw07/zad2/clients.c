#include <stdio.h>
#include <sys/param.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


#include "header.h"

int isCut = 0;
int isSeated = 0;
int cutTimes = 0;

struct queue* queuePtr = NULL;
int shmHandler = -1;
sem_t* semChair = NULL;
sem_t* semQueue = NULL;

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

    shmHandler = shm_open("/fifo", O_RDWR, 0755);
    semChair = sem_open("/chair", O_RDWR, 0755);
    semQueue = sem_open("/queue", O_RDWR, 0755);

    queuePtr = (struct queue*) mmap(NULL, sizeof(struct queue), PROT_READ | PROT_WRITE, MAP_SHARED, shmHandler, 0);

    int clientsN = atoi(argv[1]);
    int cutN = atoi(argv[2]);

    while(clientsN > 0){
        pid_t clientId = fork();

        if (clientId == 0){

            while(cutN > 0){
                //access shared memory
                sem_wait(semQueue);

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
                        sem_post(semChair);
                        isSeated = 1;
                    }
                    
                    //stop accessing shared memory
                    sem_post(semQueue);

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
                        sem_post(semQueue);

                        while(!isSeated){}
                        printf("[%ld] %d got seated.\n", timer(), getpid());
                        isSeated = 0;
                        while(!isCut){}
                        printf("[%ld] %d got cut.\n", timer(), getpid());
                        isCut = 0;
                    }else{ //else client has to leave the barbershop

                        //stop accessing shared memory
                        sem_post(semQueue);

                        printf("[%ld] No free chair for %d.\n", timer(), getpid());

                        //was not cut so he has to come back one more time
                        cutN++;
                    }
                }
                printf("[%ld] %d leaves the shop.\n", timer(), getpid());
                cutN--;
            }
            printf("[%ld] %d dies.\n",timer(), getpid());

            munmap(queuePtr, sizeof(struct queue));
            sem_close(semChair);
            sem_close(semQueue);

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
