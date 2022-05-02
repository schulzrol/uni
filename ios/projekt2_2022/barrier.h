/**
 * @file barrier.h
 * @author Roland Schulz (xschul06@stud.fit.vutbr.cz)
 * @brief Declarations for reusable barrier implementation (inspired by The Little Book of Semaphores)
 * @date 2022-05-02
 */

#ifndef PROJEKT2_2022_BARRIER_H
#define PROJEKT2_2022_BARRIER_H

#include <stdlib.h>
#include <semaphore.h>
#include "utils.h"

/**
 * @brief Reusable barrier structure
 */
struct Barrier {
    int n; /**< Number of entities to hold at the entry barrier */
    int count; /**< Number of entities that have reached the barrier */
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