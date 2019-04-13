#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

struct block{
    int size;
    char* a;
};

struct block* allocate_array(int n){
    return (struct block*) calloc(n, sizeof(struct block));
}

void deallocate_array(struct block* array,int n){
    for (int i = 0; i < n; i++){
        if(array[i].size != 0){
            free(array[i].a);
        }
    }
    free(array);
}

void allocate_block(struct block* array, int n, int i){
    array[i].size = n;
    array[i].a = (char*) calloc(n, sizeof(char));
}

void deallocate_block(struct block* array, int i){
    if (array[i].size != 0){
        array[i].size = 0;
        free(array[i].a);
    }
}

int dist(int a, int b){
    return (a-b) > 0 ? (a-b) : (b-a);
}

int find(struct block* array, int n, int number){
    int closest = INT_MAX;
    int id = -1;
    for (int i = 0; i < n; i++){
        int sum = 0;
        for (int j = 0; j < array[i].size; j++){
            sum += array[i].a[j];
        }
        if (sum == number) return i;
        if (dist(number, sum) < closest){
            closest = dist(number, sum);
            id = i;
        }
    }
    return id;
}

int insert_to_block(struct block* array, int id, char text[], int text_size){
    if (array[id].size < text_size) return -1;
    else{
        for (int i = 0; i < text_size; i++){
            array[id].a[i] = text[i];
        }
        return 0;
    }
}

void read_block(struct block* array, int id){
    for (int i = 0; i < array[id].size; i++){
        printf("%c", array[id].a[i]);
    }
    printf("\n");
}