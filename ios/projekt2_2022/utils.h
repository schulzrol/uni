//
// Created by roland on 26.4.22.
//

#ifndef PROJEKT2_2022_UTILS_H
#define PROJEKT2_2022_UTILS_H
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/file.h>
#include <sys/types.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>
#include <stdarg.h>

#include "globals.h"

typedef void (*processHandlerT)(int, long int, long int);
typedef void (*cleanupHandlerT)();

bool strtolSafe(const char* str, int* target);
bool inRangeIncluding(int value, int leftBound, int rightBound);
int rand_in_range(int min, int max);
void logEvent(const char* format, ...);
pid_t spawnProcess(int id, long int creationDelay, long int moleculeDelay, processHandlerT processHandler, cleanupHandlerT cleanupHandler);
void spawnProcesses(pid_t* returnArr,
                    int count,
                    long int creationDelay,
                    long int moleculeDelay,
                    processHandlerT processHandler,
                    cleanupHandlerT cleanupHandler);
int rand_in_range(int min, int max);
void debugEvent(const char* format, ...);
void bond(char atom, int id, long int delayinMillis);

#endif //PROJEKT2_2022_UTILS_H

