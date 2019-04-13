#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/times.h>

clock_t start_time;
clock_t end_time;
struct tms start_cpu;
struct tms end_cpu;

void clock_start(){
    start_time = times(&start_cpu);
}

void clock_end(double* res){
    
    long clk = sysconf(_SC_CLK_TCK);
    end_time = times(&end_cpu);
    res[0] = (end_cpu.tms_utime - start_cpu.tms_utime) / (double) clk;
    res[1] = (end_cpu.tms_stime - start_cpu.tms_stime) / (double) clk;

}

char* generate_string(char* buf, int string_size){
    char letters[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

    if (string_size){

        for (int i = 0; i < string_size; i++){
            int id = rand() % (int) (sizeof(letters)-1);
            buf[i] = letters[id];
        }

        buf[string_size] = '\n';

    }

    return buf;

}

void generate(char* file_name, int record_number, int record_size){
    
    int handle = open(file_name, O_RDWR|O_CREAT|O_TRUNC, 0666);
    if (handle < 0){
        printf("Could not create or open the file\n");
        return;
    }

    char* string = malloc(sizeof(char) * (record_size+1));

    for(int i = 0; i < record_number; i++){
        string = generate_string(string, record_size);
        write(handle, string, record_size+1);
    }

    free(string);

    close(handle);

}

int swap_sys(int handle, int record_x, char* str1, int record_y, char* str2, int record_size){

    lseek(handle, (record_x*(record_size+1)), SEEK_SET);
    write(handle, str2, record_size+1);
    lseek(handle, (record_y*(record_size+1)), SEEK_SET);
    write(handle, str1, record_size+1);

    return 0;

}

int swap_lib(FILE* handle, int record_x, char* str1, int record_y, char* str2, int record_size){
    
    fseek(handle, (record_x*(record_size+1)), 0);
    fwrite(str2, sizeof(char), record_size+1, handle);
    fseek(handle, (record_y*(record_size+1)), 0);
    fwrite(str1, sizeof(char), record_size+1, handle);

    return 0;

}

void sort_sys(char* file_name, int record_number, int record_size){
    
    int handle = open(file_name, O_RDWR);
    if (handle < 0){
        perror(file_name);
        return;
    }

    char* str1 = malloc(sizeof(char) * (record_size+1));
    char* str2 = malloc(sizeof(char) * (record_size+1));

    int i = 1;
    while(i < record_number){
        int j = i;
        while(j > 0){

            lseek(handle, (j * (record_size+1)), SEEK_SET);
            read(handle, str1, record_size+1);
            lseek(handle, ((j-1) * (record_size+1)), SEEK_SET);
            read(handle, str2, record_size+1);

            if ((unsigned char)str2[0] <= (unsigned char)str1[0]){
                break;
            }
            swap_sys(handle, j, str1, j-1, str2, record_size);
            j--;

        }
        i++;
    }

    free(str1);
    free(str2);

    close(handle);
}

void sort_lib(char* file_name, int record_number, int record_size){
    FILE* handle = fopen(file_name, "r+");
    if (handle == NULL){
        perror(file_name);
        return;
    }


    fseek(handle, 0, 2);
    int size = ftell(handle);
    fseek(handle, 0, 0);
    int expected_size = record_number * record_size + record_number;
    if (size != expected_size){
        printf("Record number and/or size does not match real number and/or size\n");
        fclose(handle);
        return;
    }

    char* str1 = malloc(sizeof(char) * (record_size+1));
    char* str2 = malloc(sizeof(char) * (record_size+1));

    int i = 1;
    while(i < record_number){
        int j = i;
        while(j > 0){

            fseek(handle, (j * (record_size+1)), 0);
            fread(str1, sizeof(char), record_size+1, handle);
            fseek(handle, ((j-1) * (record_size+1)), 0);
            fread(str2, sizeof(char), record_size+1, handle);

            if ((unsigned char)str2[0] <= (unsigned char)str1[0]){
                break;
            }
            swap_lib(handle, j, str1, j-1, str2, record_size);
            j--;

        }
        i++;
    }

    free(str1);
    free(str2);

    fclose(handle);
}

void copy_sys(char* file1_name, char* file2_name, int record_number, int record_size){
    int handle1 = open(file1_name, O_RDWR);
    if (handle1 < 0){
        perror(file1_name);
        return;
    }
    int handle2 = open(file2_name, O_RDWR);
    if (handle2 < 0){
        perror(file2_name);
        return;
    }

    lseek(handle2, 0, SEEK_END);
    write(handle2, "\n", 1);

    char* a = malloc(sizeof(char) * (record_size + 1));
    for(int i = 0; i < record_number; i++){
        read(handle1, a, record_size + 1);
        write(handle2, a, record_size + 1);
    }

    free(a);
    
    close(handle1);
    close(handle2);
}

void copy_lib(char* file1_name, char* file2_name, int record_number, int record_size){
    FILE* handle1 = fopen(file1_name, "r+");
    if (handle1 == NULL){
        perror(file1_name);
        return;
    }

    fseek(handle1, 0, 2);
    int size = ftell(handle1);
    fseek(handle1, 0, 0);
    int expected_size = record_number * record_size + record_number;
    if (size != expected_size){
        printf("Record number and/or size does not match real number and/or size\n");
        fclose(handle1);
        return;
    }

    FILE* handle2 = fopen(file2_name, "r+");
    if (handle2 == NULL){
        perror(file2_name);
        return;
    }

    fseek(handle2, 0, 2);
    fwrite("\n", sizeof(char), 1, handle2);

    char* a = malloc(sizeof(char) * (record_size + 1));
    for(int i = 0; i < record_number; i++){
        fread(a, sizeof(char), record_size + 1, handle1);
        fwrite(a, sizeof(char), record_size + 1, handle2);
    }

    free(a);
    
    fclose(handle1);
    fclose(handle2);
}

int main(int argc, char* argv[]){

    srand(time(NULL)); 

    if(argc < 2){
            printf("Too few arguments\n");
            return 0;
    }

    if(strcmp(argv[1], "generate") == 0){

        if (argc < 5){
            printf("Too few arguments\n");
            return 0;
        }

        if (atoi(argv[3]) == 0 || atoi(argv[4]) == 0){
            printf("Incorrect arguments\n");
            return 0;
        }

        generate(argv[2], atoi(argv[3]), atoi(argv[4]));

    }
    else if (strcmp(argv[1], "sort") == 0){

        if (argc < 6){
            printf("Too few arguments\n");
            return 0;
        }

        if (atoi(argv[3]) == 0 || atoi(argv[4]) == 0){
            printf("Incorrect arguments\n");
            return 0;
        }

        if (strcmp(argv[5], "sys") == 0){
            
            struct stat st;
            if(stat(argv[2], &st) < 0){
                perror(argv[2]);
                return 0;
            }
            int file_size = st.st_size;

            int expected_file_size = atoi(argv[3]) * atoi(argv[4]) + atoi(argv[3]);
            if (expected_file_size != file_size){
                printf("Record number and/or size does not match real number and/or size\n");
                return 0;
            }

            double* timer = malloc(2*sizeof(double));
            clock_start();
            sort_sys(argv[2], atoi(argv[3]), atoi(argv[4]));
            clock_end(timer);
            printf("sort sys: record_number = %d record_size = %d ", atoi(argv[3]), atoi(argv[4]));
            printf("user: %4.5f system: %4.5f\n", timer[0], timer[1]);
            free(timer);

        }

        else if (strcmp(argv[5], "lib") == 0){
            
            double* timer = malloc(2*sizeof(double));
            clock_start();
            sort_lib(argv[2], atoi(argv[3]), atoi(argv[4]));
            clock_end(timer);
            printf("sort lib: record_number = %d record_size = %d ", atoi(argv[3]), atoi(argv[4]));
            printf("user: %4.5f system: %4.5f\n", timer[0], timer[1]);
            free(timer);

        }
    }
    else if (strcmp(argv[1], "copy") == 0){

        if (argc < 7){
            printf("Too few arguments\n");
            return 0;
        }

        if (atoi(argv[4]) == 0 || atoi(argv[5]) == 0){
            printf("Incorrect arguments\n");
            return 0;
        }

        if (strcmp(argv[6], "sys") == 0){

            struct stat st;
            if(stat(argv[2], &st) < 0){
                perror(argv[2]);
                return 0;
            }
            int file_size = st.st_size;



            int expected_file_size = atoi(argv[4]) * atoi(argv[5]) + atoi(argv[4]);
            if (expected_file_size != file_size){
                printf("Record number and/or size does not match real number and/or size\n");
                return 0;
            }

            double* timer = malloc(2*sizeof(double));
            clock_start();
            copy_sys(argv[2], argv[3], atoi(argv[4]), atoi(argv[5]));
            clock_end(timer);
            printf("copy sys: record_number = %d record_size = %d ", atoi(argv[4]), atoi(argv[5]));
            printf("user: %4.5f system: %4.5f\n", timer[0], timer[1]);
            free(timer);
            
        }
        else if (strcmp(argv[6], "lib") == 0){
            
            double* timer = malloc(2*sizeof(double));
            clock_start();
            copy_lib(argv[2], argv[3], atoi(argv[4]), atoi(argv[5]));
            clock_end(timer);
            printf("copy lib: record_number = %d record_size = %d ", atoi(argv[4]), atoi(argv[5]));
            printf("user: %4.5f system: %4.5f\n", timer[0], timer[1]);
            free(timer);
            
        }
    }
    else{
        printf("Incorrect function name\n");
    }

    return 0;

}