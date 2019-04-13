#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <zconf.h>
#include <sys/times.h>

#include "header.h"

// global variables
pthread_mutex_t buffer_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t producer_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t consumer_cond = PTHREAD_COND_INITIALIZER;
struct Queue* global_buffer;
pthread_t* producers;
pthread_t* consumers;
FILE* text;
int flag1 = 1;
int flag2 = 1;

// program parameters
int P, K, N, L, seek_mode, write_mode, nk;

// timer
clock_t start;
clock_t end;
struct tms start_cpu;
struct tms end_cpu;

void clock_start(){
    start = times(&start_cpu);
}

void clock_end(double* res){
    long clk = sysconf(_SC_CLK_TCK);
    end = times(&end_cpu);
    *res = (end - start) / (double)clk;
}

// threads functions
void* producers_fun(){
    while(1){
        pthread_mutex_lock(&buffer_mutex);

        // check if other producer met EOF while waiting for mutex
        if (!flag1){
            pthread_mutex_unlock(&buffer_mutex);
            break;
        }

        // check if time is up, else go working
        double time;
        clock_end(&time);
        if (nk != 0 && time > (double)nk){
            // stop jobs of producers and consumers
            flag1 = 0;
            flag2 = 0;
            pthread_mutex_unlock(&buffer_mutex);
            // wake up every thread that might be asleep
            pthread_cond_broadcast(&consumer_cond);
            pthread_cond_broadcast(&producer_cond);
        }
        else {
            if (!is_full(global_buffer)) {
                // if global_buffer is not full

                char *buf;
                size_t len = 0;
                if (getline(&buf, &len, text) != -1) {
                    buf[strcspn(buf, "\n")] = 0;

                    // ignore empty lines in the file
                    if (strlen(buf) != 0) {
                        enqueue(global_buffer, buf);
                        pthread_mutex_unlock(&buffer_mutex);
                        pthread_cond_broadcast(&consumer_cond);
                        if (write_mode == 1) {
                            printf("Enqueued: %s\n", buf);
                        }
                    } else {
                        pthread_mutex_unlock(&buffer_mutex);
                    }
                } else {
                    // encountered EOF, begin finishing the job
                    if (write_mode == 1) {
                        printf("Producer encountered EOF\n");
                    }
                    flag1 = 0;
                    pthread_mutex_unlock(&buffer_mutex);

                    // wake up consumers that might be asleep
                    pthread_cond_broadcast(&consumer_cond);
                }
            } else {
                // wait till consumers freed up some space
                pthread_cond_wait(&producer_cond, &buffer_mutex);
                pthread_mutex_unlock(&buffer_mutex);
            }
        }
    }

    return NULL;
}

void* consumers_fun(){

    while(1){
        pthread_mutex_lock(&buffer_mutex);

        if(!flag2){
            // check if consumers job is done
            pthread_mutex_unlock(&buffer_mutex);
            break;
        }
        if (!is_empty(global_buffer)){
            // if global_buffer is not empty

            char buf[1024];
            int id = dequeue(global_buffer, buf);
            pthread_mutex_unlock(&buffer_mutex);

            // queue is now not empty, wake up producers that might be asleep
            pthread_cond_broadcast(&producer_cond);

            size_t length = strlen(buf);
            if (write_mode == 1){
                // always print
                printf("[%d] Consumer consumes: %s\n", id, buf);
            }
            else if (write_mode == 2){
                // print if line length meets criteria
                switch(seek_mode){
                    case -1:
                        if (length < L){
                            printf("[%d] Consumer consumes: %s\n", id, buf);
                        }
                        break;
                    case 0:
                        if (length == L){
                            printf("[%d] Consumer consumes: %s\n", id, buf);
                        }
                        break;
                    case 1:
                    default:
                        if (length > L){
                            printf("[%d] Consumer consumes: %s\n", id, buf);
                        }
                        break;
                }
            }

        }
        else{
            if (flag1 == 0){
                // check whether buffer is empty because producers finished reading the file
                flag2 = 0;
                pthread_mutex_unlock(&buffer_mutex);
            }
            else{
                // wait till buffer is not empty
                pthread_cond_wait(&consumer_cond, &buffer_mutex);
                pthread_mutex_unlock(&buffer_mutex);
            }
        }
    }

    return NULL;
}

// main
int main(int argc, char* argv[]) {

    if (argc < 2){
        printf("Too few arguments\n");
        return 1;
    }

    // read configfile

    char* setup_path = argv[1];

    FILE* setup_file = fopen(setup_path, "r");
    if (setup_file == NULL){
        perror(setup_path);
        return 1;
    }

    char* buf1 = NULL;
    size_t len = 0;

    char file_name[64];

    getline(&buf1, &len, setup_file);
    P = atoi(buf1);

    getline(&buf1, &len, setup_file);
    K = atoi(buf1);

    getline(&buf1, &len, setup_file);
    N = atoi(buf1);

    getline(&buf1, &len, setup_file);
    strcpy(file_name, buf1);
    file_name[strcspn(file_name, "\n")] = 0;

    getline(&buf1, &len, setup_file);
    L = atoi(buf1);

    getline(&buf1, &len, setup_file);
    seek_mode = atoi(buf1);

    getline(&buf1, &len, setup_file);
    write_mode = atoi(buf1);

    getline(&buf1, &len, setup_file);
    nk = atoi(buf1);

    fclose(setup_file);

    // finished reading config file
    // start producers/consumers

    producers = malloc(P * sizeof(pthread_t));
    consumers = malloc(K * sizeof(pthread_t));

    pthread_mutex_init(&buffer_mutex, NULL);
    global_buffer = create_queue(N);
    text = fopen(file_name, "r");

    clock_start();

    for(int i = 0; i < P; i++){
        pthread_create(&producers[i], NULL, producers_fun, NULL);
    }

    for(int i = 0; i < K; i++){
        pthread_create(&consumers[i], NULL, consumers_fun, NULL);
    }

    for(int i = 0; i < P; i++){
        pthread_join(producers[i], NULL);
    }

    for(int i = 0; i < K; i++){
        pthread_join(consumers[i], NULL);
    }

    // finished producers/consumers
    // clean up used resources and finish program

    fclose(text);
    free(producers);
    free(consumers);
    delete_queue(&global_buffer);

    return 0;
}