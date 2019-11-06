#include <stdio.h>
#include <stdlib.h>

int main(){
	int num = 0;
	char c;

	c = getchar();
	while (c != EOF){
		num++;
		c = getchar();
	}	

	printf("Napsal jste: %d znaku\n", num);
	return 0;
}
