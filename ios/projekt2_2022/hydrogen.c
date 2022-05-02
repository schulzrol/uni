//
// Created by roland on 26.4.22.
//

#include "hydrogen.h"

/**
 * @brief Check conditions for hydrogen atom to continue creating molecule and pass if valid. Otherwise exit.
 * 
 * @param [in] id id of the hydrogen atom
 */
void checkOxygens(int id){
    if (*oxygenLeft < 1 || *hydrogenLeft < 2){
        logEvent("H %i: not enough O or H\n", id);
        *hydrogens -= 1;
        decreaseHydrogenLeft(2);

        for (int i=0; i<*oxygenTotal; i++){
            sem_post(oxyQueue);
            sem_post(oxyQueue);
        }
        for (int i=0; i<*hydrogenTotal; i++){
            sem_post(hydroQueue);
            sem_post(hydroQueue);
        }
        sem_post(mutex);

        exit(EXIT_FAILURE);
    }
}


/**
 * @brief Handler for hydrogen atom process
 * 
 * @param [in] id id of the hydrogen atom
 * @param [in] selfCreationDelay max delay before self-creation
 * @param [in] moleculeCreationDelay max delay before molecule creation
 */
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
    else {
        checkOxygens(id);
        sem_post(mutex);
    }


    checkOxygens(id);
    sem_wait(hydroQueue); // zasekává se tady, co s tím udělat?
    //TODO: mozna udelat sem_post(hydroQueue) ve for cyklu hydroLeft-krát?
    checkOxygens(id);

    bond('H', id, rand_in_range(0, moleculeCreationDelay) * 1000);
    barrierWait(barrier);
    exit(EXIT_SUCCESS);
}
