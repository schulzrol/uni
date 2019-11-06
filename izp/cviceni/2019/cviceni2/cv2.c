#include <stdio.h>

int abs(int i){
	if (i < 0)
		i *= -1;
	return i;
}

int main(int argc, char* argv[]) {

	int i=-10;
	int j=35;

	char c='A';

	int soucet, rozdil, soucin, podil;
	soucet=i+j;
	rozdil=i-j;
	soucin=i*j;
	podil=i/j;

	/* Operace nad i,j */
	printf("j=%d\n", j);
	printf("i=%d\n", i);
	printf("soucet= %d\n", soucet);
	printf("rozdil= %d\n", rozdil);
	printf("soucin= %d\n", soucin);
	printf("podil=  %d\n", podil);
	printf("\n");

	printf("i=%d\nc=%c\n", i, c);
	printf("id=%d\ncd=%d\n", i, c);
	printf("ix=%x\ncx=%x\n", i, c);
	printf("\n");

	/* Podminky */
	if (i > j)
		printf("i je vetsi nez j\n");
	else
		if (i == j)
			printf("i je rovno j\n");
		else		
			printf("i je mensi nez j\n");
	printf("\n");

	/* Absolutni hodnota cisla */
	printf("Absolutni hodnota i= %d\n", abs(i));

	/* Prace s polem */
	int p[5] = {0};
	p[1]=12345;
	
	unsigned int n=0;
	while(n < sizeof(p)/sizeof(p[0])){
		printf("p[%u]=%d\n",n, p[n]);
		n++;
	}
	printf("\n");
	
	/* Vypsani argumentu programu */
	printf("Argumenty programu:\n");
	i=0;
	while(i < argc){
		printf("%s ", argv[i++]);
	}
	printf("\n");

	return 0;
}
