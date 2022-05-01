//
// Created by roland on 26.4.22.
//

#include "oxygen.h"

void oxygenProcessHandler(int id, long int selfCreationDelay, long int moleculeCreationDelay){
    logEvent("O %i: started\n", id);
    usleep(rand_in_range(0, selfCreationDelay) * 1000);
    logEvent("O %i: going to queue\n", id);
    sem_wait(mutex);
    *oxygens += 1;
    // pridat kontrolu pro to když zbyde oxygen/hydrogen a zasekl by se
    if (*hydrogens >= 2){
        sem_post(hydroQueue);
        sem_post(hydroQueue);
        *hydrogens -= 2;
        sem_post(oxyQueue);
        *oxygens -= 1;
    }
    else
        sem_post(mutex);

    sem_wait(oxyQueue);

    bond('O', id, rand_in_range(0, moleculeCreationDelay) * 1000);

    barrierWait(barrier);
    sem_post(mutex);
    exit(EXIT_SUCCESS);
}
