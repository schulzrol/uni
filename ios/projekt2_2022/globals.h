/**
 * @file globals.h
 * @author Roland Schulz (xschul06@stud.fit.vutbr.cz)
 * @brief Declarations of shared variables and semaphores used across project
 * @date 2022-05-02
 */

#ifndef PROJEKT2_2022_GLOBALS_H
#define PROJEKT2_2022_GLOBALS_H

#include <semaphore.h>
#include <stdio.h>
#include "barrier.h"

extern sem_t* mutex;
extern int* oxygens;
extern int* hydrogens;
extern struct Barrier* barrier;
extern sem_t* oxyQueue;
extern sem_t* hydroQueue;
extern int* moleculeCounter;
extern sem_t* incrementedMolecule;
extern sem_t* firstAtom;

extern int* oxygenTotal;
extern int* hydrogenTotal;
extern int* hydrogenLeft;
extern int* oxygenLeft;
extern sem_t* hydrogenLeftMutex;
extern sem_t* oxygenLeftMutex;

extern sem_t* printSem;
extern int* logCounter;
extern FILE* outputHandle;

#endif //PROJEKT2_2022_GLOBALS_H
