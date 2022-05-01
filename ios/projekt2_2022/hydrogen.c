//
// Created by roland on 26.4.22.
//

#include "hydrogen.h"

void hydrogenProcessHandler(int id, long int selfCreationDelay, long int moleculeCreationDelay){
    logEvent("H %i: started\n", id);
    usleep(rand_in_range(0, selfCreationDelay) * 1000);
    logEvent("H %i: going to queue\n", id);
    sem_wait(mutex);
    *hydrogens += 1;
    if (*hydrogens >= 2 && *oxygens >= 1){
        sem_post(hydroQueue);
        sem_post(hydroQueue);
        *hydrogens -= 2;
        sem_post(oxyQueue);
        *oxygens -= 1;
    }
    else
        sem_post(mutex);

    sem_wait(hydroQueue);

    bond('H', id, rand_in_range(0, moleculeCreationDelay) * 1000);

    barrierWait(barrier);
    exit(EXIT_SUCCESS);
}
