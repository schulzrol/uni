#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int* a,*b;

    a = NULL; b=malloc(sizeof(int));

    *b=14; b=a;
    *b+=*a* -5;
    printf("%d \n", *a);


    return 0;
}
