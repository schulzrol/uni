/**
 * @file oxygen.c
 * @author Roland Schulz (xschul06@stud.fit.vutbr.cz)
 * @brief Oxygen atom handler implementation
 * @date 2022-05-02
 */

#include "oxygen.h"

/**
 * @brief Check conditions for oxygen atom to continue creating molecule and pass if valid. Otherwise exit.
 * 
 * @param [in] id id of the oxygen atom
 */
void checkHydrogens(int id){
    if (*hydrogenLeft < 2){
        logEvent("O %i: not enough H\n", id);
        decreaseOxygenLeft(1);
        *oxygens -= 1;
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
 * @brief Handler for oxygen atom process
 * 
 * @param [in] id id of the oxygen atom
 * @param [in] selfCreationDelay max delay before self-creation
 * @param [in] moleculeCreationDelay max delay before molecule creation
 */
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
    else {
        checkHydrogens(id);
        sem_post(mutex);
    }

    checkHydrogens(id);
    sem_wait(oxyQueue);
    checkHydrogens(id);

    bond('O', id, rand_in_range(0, moleculeCreationDelay) * 1000);

    barrierWait(barrier);
    checkHydrogens(id);

    decreaseOxygenLeft(1);
    decreaseHydrogenLeft(2);
    sem_post(mutex);
    exit(EXIT_SUCCESS);
}
