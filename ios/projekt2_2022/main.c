//
// Created by roland on 26.4.22.
//
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <error.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <semaphore.h>

#include "utils.h"
#include "oxygen.h"
#include "hydrogen.h"
#include "barrier.h"

#define OUTPUT_FILENAME "proj2.out"

sem_t* mutex;
int* oxygens;
int* hydrogens;
struct Barrier* barrier;
sem_t* oxyQueue;
sem_t* hydroQueue;
sem_t* firstAtom;
sem_t* incrementedMolecule;

sem_t* printSem;
int* logCounter;
int* moleculeCounter;
FILE* outputHandle;


struct Arguments {
    int oxygenCount;
    int hydrogenCount;
    int maxAtomCreationMillis;
    int maxMoleculeCreationMillis;
};

bool initArguments(struct Arguments* args, int count, char** argv){
    if (count != 4)
        return false;

    if (strtolSafe(argv[0], &args->oxygenCount) ||
        strtolSafe(argv[1], &args->hydrogenCount) ||
        strtolSafe(argv[2], &args->maxAtomCreationMillis) ||
        strtolSafe(argv[3], &args->maxMoleculeCreationMillis))
        return false;

    if (! (inRangeIncluding(args->maxAtomCreationMillis, 0, 1000) &&
          (inRangeIncluding(args->maxMoleculeCreationMillis, 0, 1000))))
        return false;

    if (!((args->oxygenCount > 0) && (args->hydrogenCount > 0)))
        return false;

    return true;
}

void initSharedMemory(){
    mutex = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    printSem = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    oxyQueue = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    hydroQueue = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    barrier = mmap(NULL, sizeof(struct Barrier), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    incrementedMolecule = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    firstAtom = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);

    oxygens = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    hydrogens = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    logCounter = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    moleculeCounter = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
}

void cleanupSharedMemory(){
    munmap(mutex, sizeof (sem_t));
    munmap(barrier, sizeof (struct Barrier));
    munmap(printSem, sizeof (sem_t));
    munmap(oxyQueue, sizeof (sem_t));
    munmap(hydroQueue, sizeof (sem_t));
    munmap(incrementedMolecule, sizeof (sem_t));
    munmap(firstAtom, sizeof (sem_t));

    munmap(oxygens, sizeof (int));
    munmap(hydrogens, sizeof (int));
    munmap(moleculeCounter, sizeof (int));
    munmap(logCounter, sizeof (int));
}

void initSemaphores(){
    sem_init(mutex, 1, 1);
    initBarrier(barrier, 3);
    sem_init(printSem, 1, 1);
    sem_init(oxyQueue, 1, 0);
    sem_init(hydroQueue, 1, 0);
    sem_init(incrementedMolecule, 1, 0);
    sem_init(firstAtom, 1, 1);
}

void cleanupSemaphores(){
    sem_destroy(mutex);
    sem_destroy(printSem);
    sem_destroy(oxyQueue);
    sem_destroy(hydroQueue);
    sem_destroy(incrementedMolecule);
    sem_destroy(firstAtom);
    destroyBarrier(barrier);
}

void cleanup(){
    cleanupSemaphores();
    cleanupSharedMemory();
    if (outputHandle) fclose(outputHandle);
    // kill processes
}


void spawnerTester(int id, long int selfCreationDelay, long int moleculeCreationDelay){
    // print arguments
    debugEvent("STARTED: %i: ST: %li %li\n", id, selfCreationDelay, moleculeCreationDelay);
    logEvent("%i: ST: %li %li\n", id, selfCreationDelay, moleculeCreationDelay);
    debugEvent("ENDED: %i: ST: %li %li\n", id, selfCreationDelay, moleculeCreationDelay);
    exit(0);
}

int main(int argc, char** argv){
    struct Arguments args;
    if (!initArguments(&args, argc-1, argv+1))
        return EXIT_FAILURE;

    initSharedMemory();
    initSemaphores();

    pid_t* oxygenProcs = malloc(sizeof(pid_t) * args.oxygenCount);
    pid_t* hydrogenProcs = malloc(sizeof(pid_t) * args.hydrogenCount);

    outputHandle = fopen(OUTPUT_FILENAME, "w+");
    //setbuf(outputHandle, NULL); // vypnout bufferovani

    //spawnProcesses(NULL,
    //               args.oxygenCount,
    //               args.maxAtomCreationMillis,
    //               args.maxMoleculeCreationMillis,
    //               spawnerTester,
    //               cleanup);

    spawnProcesses(NULL,
                   args.oxygenCount,
                   args.maxAtomCreationMillis,
                   args.maxMoleculeCreationMillis,
                   oxygenProcessHandler,
                   cleanup);

    spawnProcesses(NULL,
                   args.hydrogenCount,
                   args.maxAtomCreationMillis,
                   args.maxMoleculeCreationMillis,
                   hydrogenProcessHandler,
                   cleanup);


    while (wait(NULL) > 0);
    debugEvent("All processes finished\n");

    free(oxygenProcs);
    free(hydrogenProcs);
    cleanup();
    return EXIT_SUCCESS;
}
