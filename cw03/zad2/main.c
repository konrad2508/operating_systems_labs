#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <unistd.h>
#include <sys/wait.h>

int execute_file(char* file_name){
    FILE* handle = fopen(file_name, "r");
    if (handle == NULL){
        perror(file_name);
        return 1;
    }

    char line[128];
    char line_cp[128];
    char* readed = fgets(line, 128, handle);
    strcpy(line_cp, line);
    line_cp[strcspn(line_cp, "\n")] = 0;

    int line_n = 1;
    while(readed != NULL){

        char* av[10];
        int i = 0;
        char* token = strtok(line, " ");
        while(token != NULL && i < 9){
            token[strcspn(token, "\n")] = 0;
            av[i] = token;
            token = strtok(NULL, " ");
            i++;
        }
        
        av[i] = NULL;

        int status;
        pid_t id = fork();
        if (id == 0){

            execvp(av[0], av);

            exit(1);
        }
        else{
            wait(&status);
        }

        printf("Command: %s (line %d)\n", line_cp, line_n);

        if (status != 0){
            printf("Failed to execute command (exit code %d)\n\n", status);
            fclose(handle);
            return 1;
        }
        else{
            printf("\n");
        }

        readed = fgets(line, 128, handle);
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