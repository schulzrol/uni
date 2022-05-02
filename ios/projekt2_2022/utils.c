/**
 * @file utils.c
 * @author Roland Schulz (xschul06@stud.fit.vutbr.cz)
 * @brief General utility functions used across project
 * @date 2022-05-02
 * 
 */
#include "utils.h"

/**
 * @brief Prints error message to stderr and flushes stderr
 * 
 * @param [in] format format of the message
 * @param [in] ... arguments to fill the format
 */
void errPrint(const char* format, ...){
    va_list args;
    va_start(args, format);

    vfprintf(stderr, format, args);
    fflush(stderr);

    va_end(args);
}

/**
 * @brief Converts given string @p str to integer in base 10 and stores it in @p target
 * @sideeffect @p target is overwritten with numeric value of @p str if conversion is successful
 * 
 * @param [in] str string to convert to numeric value
 * @param [out] target variable to store converted value
 * @return false when conversion was successful
 * @return true when conversion failed
 */
bool strtolSafe(const char* str, int* target){
    char * endptr;

    errno = 0;
    int value = strtol(str, &endptr, 10);
    if (errno != 0 || endptr == str)
        return true;

    *target = value;
    return false;
}

/**
 * @brief Check whether given @p value is in range [leftBound, rightBound] including both bounds
 * 
 * @param [in] value value to check
 * @param [in] leftBound left bound of range
 * @param [in] rightBound right bound of range
 * @return true when value is in range
 * @return false when value is not in range
 */
bool inRangeIncluding(int value, int leftBound, int rightBound){
    return ((value >= leftBound) && (value <= rightBound)) ? true : false;
}

/**
 * @brief Get random int between @p min and @p max
 *
 * @param min minimum value of random number
 * @param max maximum value of random number
 * @return random int between @p min and @p max
 */
int rand_in_range(int min, int max) {
    return rand()%(max-min) + min;
}

/**
 * @brief Spawns new atom process, executes given handler for atom and returns its pid
 * 
 * @param [in] id ID of new atom process
 * @param [in] creationDelay delay in milliseconds between creation of new atom process
 * @param [in] moleculeDelay delay in milliseconds between creation of new molecule
 * @param [in] processHandler function to call for new atom process
 * @param [in] cleanupHandler function to call when fork fails
 *
 * @sideeffect on failure calls @p cleanupHandler, prints error message to stderr, and exits
 *
 * @return pid_t PID of new forked process
 */
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
        errPrint("Fork failed\n");
        exit(EXIT_FAILURE);
    }

    return process;
}

/**
 * @brief Spawns new atom processes, executes given handler for atom and returns their pids in @p returnArr if not NULL
 * 
 * @param [in,out] returnArr array to store pids of new atom processes. If NULL, pids are not stored
 * @param [in] count number of new atom processes to spawn
 * @param [in] creationDelay delay in milliseconds between creation of new atom process
 * @param [in] moleculeDelay delay in milliseconds between creation of new molecule
 * @param [in] processHandler function to call for new atom process
 * @param [in] cleanupHandler function to call when fork fails
 * 
 * @sideeffect on failure calls @p cleanupHandler, prints error message to stderr, and exits
 * @sideeffect on success stores pids of new forked processes in @p returnArr if not NULL
 */
void spawnProcesses(pid_t* returnArr,
                    int count,
                    long int creationDelay,
                    long int moleculeDelay,
                    processHandlerT processHandler,
                    cleanupHandlerT cleanupHandler){
    for (int id=1; id <= count; id++){
        pid_t pid = spawnProcess(id, creationDelay, moleculeDelay, processHandler, cleanupHandler);
        //debugEvent("spawned process %i for id %i\n", pid, id);
        if (returnArr != NULL)
            returnArr[id] = pid;
    }
}

/**
 * @brief Bonding of 2H and 1O atoms into H2O molecule 
 * 
 * @sideeffect logs new molecule created to shared log file as described in @ref logEvent
 * @param [in] atom character specifying which atom informs of bonding
 * @param [in] id ID of atom
 * @param [in] delayinMillis delay in milliseconds between creation of new molecule
 */
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
    logEvent("%c %i: molecule %i created\n", atom, id, (*moleculeCounter));
    if (wasFirst) sem_post(firstAtom);
}

/**
 * @brief Logs given message to log file with log number prefix
 * 
 * @sideeffect logs message to shared log file outputHandle
 * @sideeffect increments log line counter
 * @param [in] format format of the message
 * @param [in] ...  arguments to fill the format
 */
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

/**
 * @brief Prints given message to stdout and flushes it. Mainly used for debugging
 * 
 * @param [in] format format of the message
 * @param [in] ... arguments to fill the format
 */
void debugEvent(const char* format, ...){
    va_list args;
    va_start(args, format);

    vfprintf(stdout, format, args);
    fflush(stdout);

    va_end(args);
}

/**
 * @brief Decreases the value of shared variable hydrogenLeft by @p count
 * 
 * @sideeffect decreases hydrogenLeft by @p count
 * @param [in] count number of hydrogen atoms to decrease by
 */
void decreaseHydrogenLeft(int count){
    sem_wait(hydrogenLeftMutex);
    *hydrogenLeft -= count;
    sem_post(hydrogenLeftMutex);
}

/**
 * @brief Increments the value of shared variable hydrogenLeft by @p count
 * 
 * @sideeffect increase hydrogenLeft by @p count
 * @param [in] count number of hydrogen atoms to increment by
 */
void increaseHydrogenLeft(int count){
    sem_wait(hydrogenLeftMutex);
    *hydrogenLeft += count;
    sem_post(hydrogenLeftMutex);
}

/**
 * @brief Decreases the value of shared variable oxygenLeft by @p count
 * 
 * @sideeffect decreases oxygenLeft by @p count
 * @param [in] count number of oxygen atoms to decrease by
 */
void decreaseOxygenLeft(int count){
    sem_wait(oxygenLeftMutex);
    *oxygenLeft -= count;
    sem_post(oxygenLeftMutex);
}


/**
 * @brief Increments the value of shared variable oxygenLeft by @p count
 * 
 * @sideeffect increase oxygenLeft by @p count
 * @param [in] count number of oxygen atoms to increment by
 */
void increaseOxygenLeft(int count){
    sem_wait(oxygenLeftMutex);
    *oxygenLeft += count;
    sem_post(oxygenLeftMutex);
}
