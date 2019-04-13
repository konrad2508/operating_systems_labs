#ifndef L9Z1_HEADER_H
#define L9Z1_HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

struct Queue
{
    int tail, head, size;
    int capacity;
    char** array;
};

struct Queue* create_queue(int capacity){

    struct Queue* queue = (struct Queue*) malloc(sizeof(struct Queue));
    queue->capacity = capacity;
    queue->tail = queue->size = 0;
    queue->head = capacity - 1;
    queue->array = malloc(queue->capacity * sizeof(char*));

    for(int i = 0; i < queue->capacity; i++){
        queue->array[i] = malloc(1024 * sizeof(char));
    }

    return queue;
}

void delete_queue(struct Queue** queue){
    for(int i = 0; i < (*queue)->capacity; i++){
        free((*queue)->array[i]);
    }
    free((*queue)->array);
    free(*queue);
    *queue = NULL;
}

int is_full(struct Queue *queue){
    return (queue->size == queue->capacity);
}

int is_empty(struct Queue *queue){
    return (queue->size == 0);
}

void enqueue(struct Queue* queue, char* item){

    if (is_full(queue)) {
        return;
    }

    queue->head = (queue->head + 1) % queue->capacity;
    strcpy(queue->array[queue->head], item);
    queue->size = queue->size + 1;

}

int dequeue(struct Queue* queue, char* dest){

    if (is_empty(queue)) {
        return -1;
    }

    strcpy(dest, queue->array[queue->tail]);
    int ret = queue->tail;
    queue->tail = (queue->tail + 1) % queue->capacity;
    queue->size = queue->size - 1;
    return ret;

}

#endif
