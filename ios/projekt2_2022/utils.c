//
// Created by roland on 26.4.22.
//
#include "utils.h"

bool strtolSafe(const char* str, int* target){
    char * endptr;

    errno = 0;
    int value = strtol(str, &endptr, 10);
    if (errno != 0 || endptr == str)
        return true;

    *target = value;
    return false;
}

bool inRangeIncluding(int value, int leftBound, int rightBound){
    return ((value >= leftBound) && (value <= rightBound)) ? true : false;
}

/**
 * @brief random int between min and max
 *
 * @param min
 * @param max
 * @return int random int between min and max
 */
int rand_in_range(int min, int max) {
    return rand()%(max-min) + min;
}


// possible problem s function pointrama, muzu proste udelat zvlast funkce nebo to nedelat takto zbytecne chytre
pid_t spawnProcess(int id,
                    long int creationDelay,
                    long int moleculeDelay,
                    processHandlerT processHandler,
                    cleanupHandlerT cleanupHandler){
    pid_t process = fork();
    srand(time(NULL) * getpid());
    if (process == 0) {
        processHandler(id, creationDelay, moleculeDelay);
        exit(EXIT_SUCCESS);
    }
    else if (process < 0) {
        cleanupHandler();
        exit(EXIT_FAILURE);
    }

    return process;
}

void spawnProcesses(pid_t* returnArr,
                    int count,
                    long int creationDelay,
                    long int moleculeDelay,
                    processHandlerT processHandler,
                    cleanupHandlerT cleanupHandler){
    for (int id=1; id <= count; id++){
        pid_t pid = spawnProcess(id, creationDelay, moleculeDelay, processHandler, cleanupHandler);
        if (returnArr != NULL)
            returnArr[id] = pid;
    }
}

void bond(char atom, int id, long int delayinMillis){
    bool wasFirst = false;
    if (sem_trywait(firstAtom) == 0){
        wasFirst = true;
        (*moleculeCounter)++;
        sem_post(incrementedMolecule);
        sem_post(incrementedMolecule);
    } else {
        sem_wait(incrementedMolecule);
    }

    // bond
    logEvent("%c %i: creating molecule %i\n", atom, id, (*moleculeCounter));
    usleep(delayinMillis);
    if (wasFirst) sem_post(firstAtom);
}

void logEvent(const char* format, ...) {
    sem_wait(printSem);

    va_list args;
    va_start(args, format);

    *(logCounter) += 1;
    fprintf(outputHandle, "%d: ", *(logCounter));
    vfprintf(outputHandle, format, args);

    fflush(outputHandle);
    va_end(args);
    sem_post(printSem);
}

void debugEvent(const char* format, ...){
    va_list args;
    va_start(args, format);

    vfprintf(stdout, format, args);
    fflush(stdout);

    va_end(args);
}
