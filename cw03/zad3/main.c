#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <unistd.h>
#include <sys/wait.h>

int execute_file(char* file_name, int max_time, int max_mem){
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
        struct rusage resources;
        if (id == 0){
            long bytes = (1024 * 1024) * max_mem;
            struct rlimit mem_limit;
            mem_limit.rlim_cur = bytes;
            mem_limit.rlim_max = bytes;
            setrlimit(RLIMIT_AS, &mem_limit);

            struct rlimit time_limit;
            time_limit.rlim_cur = max_time;
            time_limit.rlim_max = max_time;
            setrlimit(RLIMIT_CPU, &time_limit);

            execvp(av[0], av);

            exit(1);
        }
        else{
            wait4(id, &status, 0, &resources);
        }

        printf("Command: %s (line %d)\n", line_cp, line_n);
        printf("User time: %ld.%0*lds, System time: %ld.%0*lds, Memory: %.4lfMiB\n", resources.ru_utime.tv_sec, 6, resources.ru_utime.tv_usec,
         resources.ru_stime.tv_sec, 6, resources.ru_stime.tv_usec, (double)resources.ru_maxrss/1024);

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

    if (argc < 4){
        printf("Too few arguments\n");
        return 1;
    }

    execute_file(argv[1], atoi(argv[2]), atoi(argv[3]));

    return 0;
}