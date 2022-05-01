//
// Created by roland on 26.4.22.
//

#ifndef PROJEKT2_2022_GLOBALS_H
#define PROJEKT2_2022_GLOBALS_H

#include <semaphore.h>
#include <stdio.h>
#include "barrier.h"

// TODO: set values for globals in main
extern sem_t* mutex;
extern int* oxygens;
extern int* hydrogens;
extern struct Barrier* barrier;
extern sem_t* oxyQueue;
extern sem_t* hydroQueue;
extern int* moleculeCounter;
extern sem_t* incrementedMolecule;
extern sem_t* firstAtom;

extern sem_t* printSem;
extern int* logCounter;
extern FILE* outputHandle;

#endif //PROJEKT2_2022_GLOBALS_H
