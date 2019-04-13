#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <unistd.h>
#include <sys/wait.h>

#define BUF_SIZE 128
#define OP_COUNT 10
#define ARG_COUNT 10

int execute_file(char* file_name){
    FILE* handle = fopen(file_name, "r");
    if (handle == NULL){
        perror(file_name);
        return 1;
    }
    //czytanie linii
    char line[BUF_SIZE];
    char line_cp[BUF_SIZE];
    char* readed = fgets(line, BUF_SIZE, handle);
    strcpy(line_cp, line);
    line_cp[strcspn(line_cp, "\n")] = 0;

    int line_n = 1;
    while(readed != NULL){
        
        //wykrywanie ile programow w linii i przypisanie ich do tablicy
        int cmd_size = 0;
        char* piped[OP_COUNT];
        int i = 0;
        char* cmd = strtok(readed, "|");
        while(cmd != NULL && i < OP_COUNT){
            cmd_size++;
            size_t loc = strcspn(cmd, "\n");
            if (loc != strlen(cmd)) {
                cmd[loc] = 0;
            }
            piped[i] = cmd;
            cmd = strtok(NULL, "|");
            i++;
        }
        piped[i] = NULL;

        int* pip = NULL;
        if (cmd_size > 1) {
            pip = malloc((cmd_size-1) * 2 * sizeof(int));
            for (int i = 0; i < (cmd_size-1) * 2; i += 2) {
                pipe(pip + i);
            }
        }
        pid_t* childId = malloc(cmd_size*sizeof(pid_t));
        int* childStat = malloc(cmd_size*sizeof(int));
        for (int i = 0; i < OP_COUNT && piped[i] != NULL; i++){

            //podzial wywolania programu po spacjach
            char* av[ARG_COUNT];
            int j = 0;
            char* token = strtok(piped[i], " ");
            while(token != NULL && j < ARG_COUNT){
                size_t loc = strcspn(token, "\n");
                if (loc != strlen(token)) {
                    token[loc] = 0;
                }
                av[j] = token;
                token = strtok(NULL, " ");
                j++;
            }
            av[j] = NULL;

	        //wywolanie programu
            childId[i] = fork();
            if (childId[i] == 0){
                if (pip != NULL){
                    if (i == 0) {
                        dup2(pip[1], 1);
                    }
                    else if (i == cmd_size-1){
                        dup2(pip[2*(cmd_size-1)-2], 0);
                    }
                    else{
                        dup2(pip[i*2-2], 0);
                        dup2(pip[(i*2) + 1], 1);
                    }
                }
                if (pip != NULL){
                    for (int j = 0; j < (cmd_size-1) * 2; j++){
                        close(pip[j]);
                    }
                }

                execvp(av[0], av);
                char buf[BUF_SIZE];
                memset(buf,0,sizeof(buf));
                strcat(buf,av[0]);
                strcat(buf," error");
                perror(buf);
                exit(1);
            }
        }
        if (pip != NULL){
            for (int j = 0; j < (cmd_size-1) * 2; j++){
                close(pip[j]);
            }
        }

        for(int i = 0; i < cmd_size; i++){
            int status;
            waitpid(childId[i], &status, WUNTRACED);
            childStat[i] = status;
        }
        //sprzatanie
        free(childId);
        if (pip != NULL){
            free(pip);
        }

        //sprawdzanie czy operacja sie powiodla
        printf("Command: %s (line %d)\n", line_cp, line_n);
        if (childStat[cmd_size-1] != 0){
            printf("Failed to execute command (exit code %d)\n\n", childStat[cmd_size-1]);
            fclose(handle);

            free(childStat);

            return 1;
        }
        else{
            printf("\n");
        }

        //sprzatania ciag dalszy
        free(childStat);

        readed = fgets(line, BUF_SIZE, handle);
        strcpy(line_cp, line);
        line_cp[strcspn(line_cp, "\n")] = 0;
        line_n++;
    }

    fclose(handle);
    return 0;
}

int main(int argc, char* argv[]){

    if (argc < 2){
        printf("Too few arguments\n");
        return 1;
    }

    execute_file(argv[1]);

    return 0;
}
