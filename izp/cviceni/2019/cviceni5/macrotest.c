#include <stdio.h>

// TODO: Pouzit podobnou metodu k reseni premiove (3) funkctionality IZP projektu

#define PRERUSENE

#ifdef PRERUSENE
void funkce(){
	printf("nadefinovane\n");
}
#endif

#ifdef NEPRERUSENE
void funkce(){
	printf("NEnadefinovane\n");
}
#endif

int main(){
	
	funkce();

	return 0;
}
