#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void myswap(int* a, int* b){
	int tmp = *a;
	*a = *b;
	*b = tmp;
}

int main(){
	int a = 43;
	int b = 68;
	
	int* pa = &a;
	int* pb = &b;

	printf("a = %d\n", a);
	printf("b = %d\n", b);
	printf("pa = %d\n", *pa);
	printf("pb = %d\n", *pb);
	
	myswap(pa, pb);


	printf("\n");
	printf("Po vymene:\n");
	
	printf("a = %d\n", a);
	printf("b = %d\n", b);
	printf("pa = %d\n", *pa);
	printf("pb = %d\n", *pb);
	

	printf("pred a = %d\n", a);
	a = *++pa;
	printf("po a = %d\n", a);

	return 0;
}

