#include <stdio.h>

int a = 4, b = 5;

void test(int *c){
    int a = *c; printf("%d %d ", a, b);
    if (a > b) { a = b; } else {int b = a; printf("%d %d ", a, b);}
    *c = a;

}

int main(void) {
    int b = 6; printf("%d %d ", a, b);
    test(&b);  printf("%d %d ", a, b);

    return 0;
}
