#include <stdio.h>

int add(int a, int b){
return a + b;
}

int main(void){
    int counter = 0;

    for (int i = 9; --i; counter++)
        printf("hodnota i je %d \n", i--);
    printf("pocet iteraci je %d \n", counter);
    return 0;
}
