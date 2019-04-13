#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <pthread.h>
#include <math.h>
#include <fcntl.h>
#include <time.h>
#include <sys/times.h>
#include <unistd.h>


int threads;
int W;
int H;
int c;
int** imageMatrix;
double** filterMatrix;
int** filteredImageMatrix;

clock_t start;
clock_t end;
struct tms startCpu;
struct tms endCpu;

void clock_start(){
    start = times(&startCpu);
}

void clock_stop(double* res){
    long clk = sysconf(_SC_CLK_TCK);
    end = times(&endCpu);
    res[0] = (end - start) / (double)clk;
    res[1] = (endCpu.tms_utime - startCpu.tms_utime) / (double)clk;
    res[2] = (endCpu.tms_stime - startCpu.tms_stime) / (double)clk;
}

int max(int a, int b){
    return a >= b ? a : b;
}

int min(int a, int b){
    return a < b ? a : b;
}

void* thread_fun(void* arg){
    int threadRange = H / threads;
    int threadNum = *((int*) arg);
    for (int x = threadNum * threadRange; x < (threadNum+1)*threadRange; x++){
        for (int y = 0; y < W; y++){
            double s = 0;
            for (int i = 0; i < c; i++){
                for(int j = 0; j < c; j++){
                    //for edge handling extend border pixels as far as necessary - 'Extend' method
                    int id1 = min(max(0, x - (int) ceil((double)c/2) + i), (threadNum+1)*threadRange - 1);
                    int id2 = min(max(0, y - (int) ceil((double)c/2) + j), W-1);
                    s += imageMatrix[id1][id2] * filterMatrix[i][j];
                }
            }
            filteredImageMatrix[x][y] = (int) round(s);
        }
    }
    free(arg);
    return NULL;
}

int main(int argc, char* argv[]) {

    if (argc < 5){
        printf("Too few arguments\n");
        return 1;
    }

    //program parameters
    threads = atoi(argv[1]);
    char* img = argv[2];
    char* filter = argv[3];
    char* out = argv[4];
    //variables for getline()
    char* line = NULL;
    size_t len = 0;

    //read input file
    FILE* input = fopen(img, "r");
    if (input == NULL){
        perror(img);
        return 1;
    }
    getline(&line, &len, input); //skip 'P2' heading
    getline(&line, &len, input); //W and H
    W = atoi(strtok(line, " "));
    H = atoi(strtok(NULL, " "));
    getline(&line, &len, input); // skip 'M' - assume M = 255

    //read and save image to imageMatrix
    imageMatrix = malloc(H * sizeof(int*));
    for (int i = 0; i < H; i++){
        getline(&line, &len, input);
        imageMatrix[i] = malloc(W * sizeof(int));
        char* token = strtok(line, " ");
        for(int j = 0; j < W; j++){
            imageMatrix[i][j] = atoi(token);
            token = strtok(NULL, " ");
        }
    }
    fclose(input);

    //read filter
    FILE* flt = fopen(filter, "r");
    if (flt == NULL){
        perror(filter);
        return 1;
    }
    getline(&line, &len, input);
    c = atoi(strtok(line, " "));
    filterMatrix = malloc(c * sizeof(double*));
    for (int i = 0; i < c; i++){
        getline(&line, &len, input);
        filterMatrix[i] = malloc(c * sizeof(double));
        char* token = strtok(line, " ");
        for(int j = 0; j < c; j++){
            filterMatrix[i][j] = atof(token);
            token = strtok(NULL, " ");
        }
    }
    fclose(flt);

    //allocate filtered image matrix
    filteredImageMatrix = malloc(H * sizeof(int*));
    for(int i = 0; i < H; i++){
        filteredImageMatrix[i] = malloc(W*sizeof(int));
    }

    //start thread job
    clock_start();
    pthread_t* threadsArr = malloc(threads * sizeof(pthread_t));
    for (int i = 0; i < threads; i++){
        int* arg = malloc(sizeof(int));
        *arg = i;
        pthread_create(&threadsArr[i], NULL, thread_fun, (void*) arg);
    }

    for(int i = 0; i < threads; i++){
        pthread_join(threadsArr[i], NULL);
    }
    double clk[3];
    clock_stop(clk);
    printf("Source: %s, %d threads, %dx%d kernel\nReal time: %lf\n\n", img, threads, c, c, clk[0]);

    //save result
    FILE* res = fopen(out, "w");
    if (res == NULL){
        perror(out);
        return 1;
    }

    fprintf(res, "P2\n%d %d\n%d\n", W, H, 255);
    for (int i = 0; i < H; i++){
        for(int j = 0; j < W; j++){
            fprintf(res, "%d ", filteredImageMatrix[i][j]);
        }
        fprintf(res, "\n");
    }
    fclose(res);

    //free
    for (int i = 0; i < H; i++){
        free(imageMatrix[i]);
    }
    free(imageMatrix);
    for(int i = 0; i < c; i++){
        free(filterMatrix[i]);
    }
    free(filterMatrix);
    for(int i = 0; i < H; i++){
        free(filteredImageMatrix[i]);
    }
    free(filteredImageMatrix);
    free(threadsArr);

    return 0;
}
