/**
 * @file main.c
 * @author Roland Schulz (xschul06@stud.fit.vutbr.cz)
 * @brief H20 creation simulation - IOS project 2
 * @date 2022-05-02
 */

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
int* hydrogenLeft;
int* oxygenLeft;
int* oxygenTotal;
int* hydrogenTotal;
struct Barrier* barrier;
sem_t* oxyQueue;
sem_t* hydroQueue;
sem_t* firstAtom;
sem_t* incrementedMolecule;
sem_t* hydrogenLeftMutex;
sem_t* oxygenLeftMutex;

sem_t* printSem;
int* logCounter;
int* moleculeCounter;
FILE* outputHandle;

/**
 * @brief Structure for storing commandline arguments
 */
struct Arguments {
    int oxygenCount; //!< Number of oxygen molecules
    int hydrogenCount; //!< Number of hydrogen molecules
    int maxAtomCreationMillis; //!< Maximum time between atom creation
    int maxMoleculeCreationMillis; //!< Maximum time between molecule creation
};

/**
 * @brief Initialize arguments from given arguments (expects argc-1 and argv+1)
 * 
 * @param [out] args structure to store arguments in
 * @param [in] count number of arguments
 * @param [in]  argv array of arguments
 * @return true when all arguments are valid
 * @return false when invalid arguments

 * @sideeffect args is filled with arguments
 * @sideeffect on invalid input, prints to stderr the corresponding error message
 */
bool initArguments(struct Arguments* args, int count, char** argv){
    if (count != 4){
        errPrint("Not enough or too much arguments\n");
        return false;
    }

    if (strtolSafe(argv[0], &args->oxygenCount) ||
        strtolSafe(argv[1], &args->hydrogenCount) ||
        strtolSafe(argv[2], &args->maxAtomCreationMillis) ||
        strtolSafe(argv[3], &args->maxMoleculeCreationMillis)){
        errPrint("Input arguments not numeric\n");
        return false;
    }

    if (! (inRangeIncluding(args->maxAtomCreationMillis, 0, 1000) &&
          (inRangeIncluding(args->maxMoleculeCreationMillis, 0, 1000)))){
        errPrint("Invalid range for atom or molecule delay, must be in (0, 1000)\n");
        return false;
    }

    if (!((args->oxygenCount > 0) && (args->hydrogenCount > 0))){
        errPrint("Invalid number of atoms, must be positive\n");
        return false;
    }

    return true;
}

/**
 * @brief Initialize shared memory for semaphores and variables
 */
void initSharedMemory(){
    mutex = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    printSem = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    oxyQueue = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    hydrogenLeftMutex = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    oxygenLeftMutex = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    hydroQueue = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    barrier = mmap(NULL, sizeof(struct Barrier), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    incrementedMolecule = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    firstAtom = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);

    oxygenTotal = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    hydrogenTotal = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    oxygens = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    oxygenLeft = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    hydrogenLeft = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    hydrogens = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    logCounter = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    moleculeCounter = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
}


/**
 * @brief Free shared memory
 */
void cleanupSharedMemory(){
    munmap(mutex, sizeof (sem_t));
    munmap(barrier, sizeof (struct Barrier));
    munmap(printSem, sizeof (sem_t));
    munmap(oxyQueue, sizeof (sem_t));
    munmap(hydroQueue, sizeof (sem_t));
    munmap(incrementedMolecule, sizeof (sem_t));
    munmap(firstAtom, sizeof (sem_t));

    munmap(oxygenTotal, sizeof (int));
    munmap(hydrogenTotal, sizeof (int));
    munmap(oxygens, sizeof (int));
    munmap(hydrogens, sizeof (int));
    munmap(moleculeCounter, sizeof (int));
    munmap(logCounter, sizeof (int));
}

/**
 * @brief Initialize shared semaphores and barriers
 */
void initSemaphores(){
    sem_init(mutex, 1, 1);
    initBarrier(barrier, 3);
    sem_init(printSem, 1, 1);
    sem_init(oxyQueue, 1, 0);
    sem_init(hydroQueue, 1, 0);
    sem_init(incrementedMolecule, 1, 0);
    sem_init(firstAtom, 1, 1);
    sem_init(oxygenLeftMutex, 1, 1);
    sem_init(hydrogenLeftMutex, 1, 1);
}

/**
 * @brief Destroy shared semaphores and barriers
 */
void cleanupSemaphores(){
    sem_destroy(mutex);
    sem_destroy(printSem);
    sem_destroy(oxyQueue);
    sem_destroy(hydroQueue);
    sem_destroy(incrementedMolecule);
    sem_destroy(firstAtom);
    sem_destroy(hydrogenLeftMutex);
    sem_destroy(oxygenLeftMutex);
    destroyBarrier(barrier);
}

/**
 * @brief Cleanup resources used by the program
 */
void cleanup(){
    cleanupSemaphores();
    cleanupSharedMemory();
    if (outputHandle) fclose(outputHandle);
}

int main(int argc, char** argv){
    struct Arguments args;
    if (!initArguments(&args, argc-1, argv+1))
        return EXIT_FAILURE;


    initSharedMemory();
    initSemaphores();

    increaseHydrogenLeft(args.hydrogenCount);
    increaseOxygenLeft(args.oxygenCount);
    *hydrogenTotal = args.hydrogenCount;
    *oxygenTotal = args.oxygenCount;

    outputHandle = fopen(OUTPUT_FILENAME, "w+");
    //setbuf(outputHandle, NULL); // vypnout bufferovani

    // spawn oxygen processes
    spawnProcesses(NULL,
                   args.oxygenCount,
                   args.maxAtomCreationMillis,
                   args.maxMoleculeCreationMillis,
                   oxygenProcessHandler,
                   cleanup);

    // spawn hydrogen processes
    spawnProcesses(NULL,
                   args.hydrogenCount,
                   args.maxAtomCreationMillis,
                   args.maxMoleculeCreationMillis,
                   hydrogenProcessHandler,
                   cleanup);


    // wait until all processes are finished
    while (wait(NULL) > 0);

    cleanup();
    return EXIT_SUCCESS;
}
