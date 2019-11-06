#include <stdio.h>
#define NUL '\0'

int my_abs(int x){
	return (x>0) ? x : -x;
}

int my_max(int a, int b){
	return (a>=b) ? a : b;
}

int my_crazy_min(int a, int b){
	return (a<=b) ? my_abs(a) : my_abs(b);
}

int my_strlen(char str[]){
	int delka = 0;
	while((str != NULL) && *(str++) != NUL) {
		delka++;
	}
	
	return delka;
}

int my_strchr(char str[], char ch){
	int oci = 0;
	while((str != NULL) && *(str++) != ch) {
		oci++;
	}
	
	return (*(--str)==ch) ? oci : (ch == NUL) ? oci : -1;
}

int my_strrchr(char str[], char ch){
	int loci = -1;
	int i = 0;

	while((str != NULL) && *(str++) != NUL) {
		i++;
		if (*(str-1) == ch)
			loci = i-1;
	}
	
	return loci;
}

char *hodiny2str(int hodin){
	if (hodin >= 5) {
		return "hodin";
	}
	else if (hodin >= 2) {
		return "hodiny";
	}
	return "hodina";
}

char *minuta2str(int minut){
	if (minut >= 5) {
		return "minut";
	}
	else if (minut >= 2) {
		return "minuty";
	}
	return "minuta";
}

int main(){

	int a = -1;
	int b = 2;
	printf("abs(%d) = %d\n", a, my_abs(a));

	printf("Vetsi z %d a %d je %d\n",a , b, my_max(a,b));
	
	printf("Crazy_min(%d, %d) = %d\n", a, b, my_crazy_min(a,b));

	char str[] = "Ahoj svete!";
	printf("Delka \"%s\" je %d\n", str, my_strlen(str));
	
	char znak = 'e';
	printf("Prvni vyskyt \'%c\' v \"%s\" je na %d. pozici\n", znak, str, my_strchr(str, znak));
		
	printf("Posledni  vyskyt \'%c\' v \"%s\" je na %d. pozici\n", znak, str, my_strrchr(str, znak));
	
	int hodin = 3;
	printf("%d %s\n", hodin, hodiny2str(hodin));

	int minut = 25;
	printf("%d %s\n", minut, minuta2str(minut));
	return 0;
}
