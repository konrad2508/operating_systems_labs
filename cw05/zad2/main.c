#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <wait.h>

int main(int argc, char* argv[]) {

    if (argc < 4){
        printf("Too few arguments\n");
        return 1;
    }

    int slaves = atoi(argv[2]);

    if (slaves < 1){
        printf("Incorrect number of slaves\n");
        return 1;
    }
    if (atoi(argv[3]) < 1){
        printf("Incorrect number of messages\n");
        return 1;
    }
    pid_t* slaveId = malloc(slaves * sizeof(pid_t));

    mkfifo(argv[1], 0755);

    for(int i = 0; i < slaves; i++){
        slaveId[i] = fork();
        if (slaveId[i] == 0){
            execlp("./slave", "slave", argv[1], argv[3], NULL);
            perror("execlp slave failed");
            exit(1);
        }
    }

    pid_t masterId = fork();
    if (masterId == 0){
        execlp("./master", "master", argv[1], NULL);
        perror("execlp master failed");
        exit(1);
    }

    for (int i = 0; i < slaves; i++){
        waitpid(slaveId[i], NULL, WUNTRACED);
    }

    free(slaveId);

    waitpid(masterId, NULL, WUNTRACED);

    remove(argv[1]);

    return 0;
}