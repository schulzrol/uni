/**
 * @file proj2.c
 * @author Rolans Schulz (xschul06 at fit.stud.vutbr.com)
 * @brief Main program for Santa Clasu problem assignement
 * @version 0.1
 * @date 2021-04-19
 */

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <limits.h>
#include <errno.h> // 
#include <semaphore.h> // semaphores, duh 💁
#include <unistd.h> // forking and children

#define log_file "proj2.out"

typedef struct _ParamsType {
    int NE; // pocet skritku 0<NE<1000 - NElves
    int NR; // poset sobu 0<NR<20 - NReindeers
    int TE; // max doba v [ms] trvani samostatne prace skritka 0<=TE<=1000
    int TR; // max doba v [ms] trvani sobi dovolene 0<=TR<=1000
} ParamsType;

#define exit_err 1
#define exit_ok 0

#define strtol_checked(p, i) do {                     \
        p = strtol(argv[i], NULL, 10);\
        if (errno == ERANGE) return exit_err;\
    } while(0);

int loadParams(int argc, char** argv, ParamsType* params){
    if (argc < 5) // not enough args
        return exit_err;
    
    strtol_checked(params->NE, 1);
    strtol_checked(params->NR, 2);
    strtol_checked(params->TE, 3);
    strtol_checked(params->TR, 4);

    if ((params->NE <= 0 || params->NE >= 1000) ||
        (params->NR <= 0 || params->NR >= 20)   ||
        (params->TE <  0 || params->TE >  1000) ||
        (params->TR <  0 || params->TR >  1000))
        return exit_err;
    
    return exit_ok;
}


int elves_counter = 0;
int reindeer_counter = 0;

int main(int argc, char** argv) {
    printf("HELLO WORLD\n");
    ParamsType params;
    if (loadParams(argc, argv, &params)) {
        fprintf(stderr, "chyba voe\n");
        return exit_err;
    }
    printf("%d %d %d %d\n", params.NE, params.NR, params.TE, params.TR);
    return exit_ok;
}