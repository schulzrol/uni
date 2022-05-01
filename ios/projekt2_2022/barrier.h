
#ifndef PROJEKT2_2022_BARRIER_H
#define PROJEKT2_2022_BARRIER_H

#include <stdlib.h>
#include <semaphore.h>
#include "utils.h"

struct Barrier {
    int n;
    int count;
    sem_t mutex;
    sem_t turnstile;
    sem_t turnstile2;
};

void initBarrier(struct Barrier* barrier, int n);

void phase1(struct Barrier* barrier);

void phase2(struct Barrier* barrier);

void barrierWait(struct Barrier* barrier);

void destroyBarrier(struct Barrier* barrier);

#endif //PROJEKT2_2022_BARRIER_H