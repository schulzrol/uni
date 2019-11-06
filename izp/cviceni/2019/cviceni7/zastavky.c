#define MAX_STR_LEN 255
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	int hodin;
	int minut;
} cas_T;

// Typ zastavky
typedef struct _zastavkaT{
	char nazev[MAX_STR_LEN + 1];
	cas_T cas_jizdy;
} zastavkaT;

// Typ linky
typedef struct _busT{
	int nzastavek;
	zastavkaT* zastavka;

	int nspoju;
	cas_T* spoj;
} busT;

busT load(FILE* f) {
	busT linka;

	// nacteni poctu zastavek
	fscanf(f, "%d", &linka.nzastavek);
	linka.zastavka = malloc(sizeof(*linka.zastavka) * linka.nzastavek);
	
	// nacteni casu dojezdu do zastavek od startu
	for (int i = 0; i < linka.nzastavek; i++) {
		int tmpminut;
		fscanf(f, "%s %d", linka.zastavka[i].nazev, &tmpminut);
			
		// konvertovani minut na hodiny a minuty
		linka.zastavka[i].cas_jizdy.hodin = tmpminut/60;
		linka.zastavka[i].cas_jizdy.minut = tmpminut%60;
	}

	// nacteni poctu spoju
	fscanf(f, "%d", &linka.nspoju);
	linka.spoj = malloc(sizeof(*linka.spoj) * linka.nspoju);
	
	// nacteni vyjezdu spoju
	for (int i = 0; i < linka.nspoju; i++) {
		fscanf(f, "%dhod %dmin", &linka.spoj[i].hodin, &linka.spoj[i].minut);
	}
	
	return linka;
}

// vypis trvani jizdy od startu k zastavce
void print_bus(busT* busptr) {

	for(int i = 0; i < busptr->nzastavek; i++) {
		printf("Zastavka \"%s\" doba do dojezdu %d:%d\n", busptr->zastavka[i].nazev, busptr->zastavka[i].cas_jizdy.hodin, busptr->zastavka[i].cas_jizdy.minut);
	}
	return;	
}

// vypis prijezdu urciteho spoje na zastavce
void print_spoje(busT* busptr, char* zastavka, cas_T cas) {
	int idzastavky = -1;

	// najit index zastavky podle jmena
	for(int i = 0; i < busptr->nzastavek; i++) {
		if (!strcmp(busptr->zastavka[i].nazev, zastavka)){
			idzastavky = i;
			break;
		}
	}	

	// nenasli sme zastavku ve spoji
	if (idzastavky == -1)
		return;

	// vypsani prijezdu na zastavku
	for (int i = 0; i < busptr->nspoju; i++){
		// uz si nepamatuju na co ten cas chtel
		(void) cas;

        // melo by vypocitat pripady, kdy by hodina navysila den a minuta navysila hodinu
        // TODO neotestovano
		int soucet_minut = busptr->spoj[i].minut + busptr->zastavka[idzastavky].cas_jizdy.minut;
		int soucet_hodin = busptr->spoj[i].hodin + busptr->zastavka[idzastavky].cas_jizdy.hodin + soucet_minut/60;
		soucet_hodin = soucet_hodin%24;
		soucet_minut = soucet_minut%60;

        // formatovany vypis
		printf("Spoj %d:%d bude na zastavce \"%s\" v %d:%d\n", busptr->spoj[i].hodin, busptr->spoj[i].minut, zastavka, soucet_hodin, soucet_minut);
	}

	return;
}

void free_bus(busT* bus){
	free(bus->spoj);

	free(bus->zastavka);
}

int main(int argc, char **argv){
	if (argc != 2) {
		fprintf(stderr, "%s: Nedostatek argumentu (Chybi nazev souboru)\n", argv[0]);
		return 1;
	}
	FILE* f = fopen(argv[1], "r");

	busT linka = load(f); 
	print_bus(&linka);

	cas_T cas;
	cas.hodin = 8;
	cas.minut = 10;

	print_spoje(&linka, linka.zastavka[3].nazev, cas);

	free_bus(&linka);
	return 0;
}
