#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int bit_lower(int c){
    return ('A' <= c && c <= 'Z')*(c|1<<5) +
           (c < 'A' || 'Z' < c)*c;
}

int main(int argc, char** argv){
    clock_t start, end;
    int c = getchar();


    start = clock();
    while(c != EOF){
        putchar(bit_lower(c));
        c = getchar();
    }
    end = clock();

    double time_taken = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf(">> took %f seconds to execute\n", time_taken);

    return 0;
}
