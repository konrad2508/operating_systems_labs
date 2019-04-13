#include <stdio.h>
#include <sys/param.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>

#include "header.h"

struct queue* queuePtr = NULL;
int shmHandler = -1;
sem_t* semChair = NULL;
sem_t* semQueue = NULL;

void sigtermHandler(){
    munmap(queuePtr, sizeof(struct queue));
    shm_unlink("/fifo");
    sem_close(semChair);
    sem_unlink("/chair");
    sem_close(semQueue);
    sem_unlink("/queue");
    exit(0);
}

int main(int argc, char* argv[]) {

    if (argc < 2){
        printf("Too few arguments\n");
        return -1;
    }

    signal(SIGTERM, sigtermHandler);
    signal(SIGINT, sigtermHandler);

    shmHandler = shm_open("/fifo", O_CREAT | O_EXCL | O_RDWR, 0755);
    ftruncate(shmHandler, sizeof(struct queue));

    semChair = sem_open("/chair", O_CREAT | O_EXCL | O_RDWR, 0755, 0);
    semQueue = sem_open("/queue", O_CREAT | O_EXCL | O_RDWR, 0755, 1);


    queuePtr = (struct queue*) mmap(NULL, sizeof(struct queue), PROT_READ | PROT_WRITE, MAP_SHARED, shmHandler, 0);
    initQueue(queuePtr, atoi(argv[1]));

    while(1){

        //barber sleeps
        printf("[%ld] Barber sleeps.\n", timer());
        
        //wait until someone has taken the chair
        sem_wait(semChair);

        //get pid of the dude in the chair
        sem_wait(semQueue);
        pid_t chairPid = queuePtr->chair;
        sem_post(semQueue);

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
        sem_wait(semQueue);
        leaveChair(queuePtr);
        sem_post(semQueue);

        while(1){//cut everyone in the queue

            sem_wait(semQueue);
            pid_t toCut = pop(queuePtr);
            sem_post(semQueue);

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
