#include "barrier.h"

void initBarrier(struct Barrier* barrier, int n){
    barrier->n = n;
    barrier->count = 0;
    sem_init(&barrier->mutex, 1, 1);
    sem_init(&barrier->turnstile, 1, 0);
    sem_init(&barrier->turnstile2, 1, 0);
}

void phase1(struct Barrier* barrier){
    sem_wait(&barrier->mutex);
    barrier->count++;
    if (barrier->count == barrier->n){
        for (int i = 0; i < barrier->n; i++) sem_post(&barrier->turnstile);
    }

    sem_post(&barrier->mutex);
    sem_wait(&barrier->turnstile);
}

void phase2(struct Barrier* barrier){
    sem_wait(&barrier->mutex);
    barrier->count--;
    if (barrier->count == 0){
        for (int i = 0; i < barrier->n; i++) sem_post(&barrier->turnstile2);
    }

    sem_post(&barrier->mutex);
    sem_wait(&barrier->turnstile2);
}

void destroyBarrier(struct Barrier* barrier){
    sem_destroy(&barrier->mutex);
    sem_destroy(&barrier->turnstile);
    sem_destroy(&barrier->turnstile2);
}

// wait function for barrier
void barrierWait(struct Barrier* barrier){
    phase1(barrier);
    phase2(barrier);
}