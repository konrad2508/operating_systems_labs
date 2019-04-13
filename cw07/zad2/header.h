#ifndef HEADER_H
#define HEADER_H

#include <time.h>

#define MAX_LOBBY 128
#define CHAIR 0
#define QUEUE 1
#define SYNC 2

long timer(){
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_nsec / 1000;
}

//queue
struct queue{
    int head;
    int tail;
    int curr_size;
    int max_size;
    pid_t lobby[MAX_LOBBY];
    pid_t chair;
};

int isFull(struct queue* queuePointer){
    return queuePointer->curr_size == queuePointer->max_size;
}

int isEmpty(struct queue* queuePointer){
    return queuePointer->curr_size == 0;
}

void initQueue(struct queue* queuePointer, int size){
    queuePointer->head = 0;
    queuePointer->tail = -1;
    queuePointer->curr_size = 0;
    queuePointer->max_size = size;
    queuePointer->chair = -1;
}

void sit(struct queue* queuePointer, pid_t pid){
    queuePointer->chair = pid;
}

void leaveChair(struct queue* queuePointer){
    queuePointer->chair = -1;
}

int push(struct queue* queuePointer, pid_t pid){
    if (queuePointer->curr_size == queuePointer->max_size){
        return -1;
    }
    queuePointer->tail++;
    queuePointer->tail %= MAX_LOBBY;
    queuePointer->lobby[queuePointer->tail] = pid;
    queuePointer->curr_size++;
    return 0;
}

pid_t pop(struct queue* queuePointer){
    if (isEmpty(queuePointer)){
        return -1;
    }
    pid_t ret = queuePointer->lobby[queuePointer->head];
    queuePointer->head++;
    queuePointer->head %= MAX_LOBBY;
    queuePointer->curr_size--;
    return ret;
}

#endif //L7Z1_HEADER_H
