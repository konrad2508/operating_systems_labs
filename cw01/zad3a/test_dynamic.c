#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/times.h>
#include <unistd.h>
#include <dlfcn.h>


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
    res[0] = (end - start) / (double)clk;
    res[1] = (end_cpu.tms_utime - start_cpu.tms_utime) / (double)clk;
    res[2] = (end_cpu.tms_stime - start_cpu.tms_stime) / (double)clk;
}

int main(int argc, char* argv[]){
    
    if (argc < 3) return 0;

    int number_of_elements = atoi(argv[1]);
    int block_size = atoi(argv[2]);
    if (number_of_elements == 0 || block_size == 0) return 0;

    void* handle;
    handle = dlopen("./liblib1.so", RTLD_LAZY);
    if (!handle) {
            fputs (dlerror(), stderr);
            exit(1);
    }
    struct block* (*allocate_array)(int);
    void (*allocate_block)(struct block*, int, int);
    int (*insert_to_block)(struct block*, int, char[], int);
    int (*find)(struct block*, int, int);
    void (*deallocate_block)(struct block*, int);

    allocate_array = dlsym(handle, "allocate_array");
    allocate_block = dlsym(handle, "allocate_block");
    insert_to_block = dlsym(handle, "insert_to_block");
    find = dlsym(handle, "find");
    deallocate_block = dlsym(handle, "deallocate_block");

    srand(time(NULL));
    
    struct block* array;
    int allocated = 0;

    printf("number_of_elements = %d, block_size = %d\n", number_of_elements, block_size);

    for (int i = 3; i < argc; i++){
        double times[3];
        switch(atoi(argv[i])){
            
            case 1:
            if (allocated == 0){
                allocated = 1;
                clock_start();
                array = (*allocate_array)(number_of_elements);
                for (int j = 0; j < number_of_elements; j++){
                    (*allocate_block)(array, block_size, j);
                    char* text = malloc(block_size * sizeof(char));

                    for (int k = 0; k < block_size; k++){
                        text[k] = "" + (rand() % (122 + 1 - 65) + 65);
                    }
                    (*insert_to_block)(array, j, text, block_size);
                    free(text);
                }
                clock_end(times);
                printf("allocating array\nreal:%4.3f user:%4.3f system:%4.3f\n", times[0], times[1], times[2]);
            }
            break;

            case 2:
            if (allocated == 1){
                clock_start();
                i++;
                char* argument = argv[i];
                int id = (*find)(array, number_of_elements, strlen(argument));
                clock_end(times);
                printf("find in array\nreal:%4.3f user:%4.3f system:%4.3f\n", times[0], times[1], times[2]);
            }
            break;

            case 3:
            if (allocated == 1){
                clock_start();
                for (int j = 0; j < number_of_elements; j++){
                    (*deallocate_block)(array, j);
                }
                for (int j = 0; j < number_of_elements; j++){
                    (*allocate_block)(array, block_size, j);
                    char* text = malloc(block_size * sizeof(char));

                    for (int k = 0; k < block_size; k++){
                        text[k] = "" + (rand() % (122 + 1 - 65) + 65);
                    }
                    (*insert_to_block)(array, j, text, block_size);
                    free(text);
                }
                clock_end(times);
                printf("deallocate method 1\nreal:%4.3f user:%4.3f system:%4.3f\n", times[0], times[1], times[2]);
            }
            break;

            case 4:
            if (allocated == 1){
                clock_start();
                for (int j = 0; j < number_of_elements; j++){
                    (*deallocate_block)(array, j);
                    (*allocate_block)(array, block_size, j);
                    char* text = malloc(block_size * sizeof(char));

                    for (int k = 0; k < block_size; k++){
                        text[k] = "" + (rand() % (122 + 1 - 65) + 65);
                    }
                    (*insert_to_block)(array, j, text, block_size);
                    free(text);
                }
                clock_end(times);
                printf("deallocate method 2\nreal:%4.3f user:%4.3f system:%4.3f\n", times[0], times[1], times[2]);
            }
            break;

        }
    }

    printf("\n");

    dlclose(handle);

    return 0;
}