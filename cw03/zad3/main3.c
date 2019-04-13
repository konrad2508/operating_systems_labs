#include <stdlib.h>
#include <stdio.h>

void allocate(){
    long long int** testArr = malloc(10000000 * sizeof(long long int*));
    for(int i = 0; i < 10000000; i++) {
        testArr[i] = malloc(10000000 * sizeof(long long int));
    }
}

int main(){

    while(1){
        allocate();
    }

    return 0;
}