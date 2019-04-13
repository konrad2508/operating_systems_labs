#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 1024

int main(int argc, char* argv[]){

    FILE* fifo = fopen(argv[1], "r");
    char buf[BUF_SIZE];

    char* readed = fgets(buf, BUF_SIZE, fifo);
    while(readed != NULL){
        printf("%s", readed);
        readed = fgets(buf, BUF_SIZE, fifo);
    }
    fclose(fifo);

    return 0;
}