#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>

#define BUF_SIZE 1024

int main(int argc, char* argv[]){

    srand((unsigned int)time(NULL));
    FILE* fifo = fopen(argv[1], "w");

    printf("slave pid= %d\n", getpid());

    for (int i = 0; i < atoi(argv[2]); i++){

        char buf[BUF_SIZE];

        FILE* date = popen("date", "r");

        fgets(buf, BUF_SIZE, date);
        char buf2[BUF_SIZE];
        sprintf(buf2, "pid= %d, date= %s", getpid(), buf);
        fwrite(buf2, sizeof(char), strlen(buf2), fifo);
        pclose(date);

        sleep(rand()%4+2);

    }

    fclose(fifo);

    return 0;
}
