/**
 * @file proj2.c
 * @author Roland Schulz (xschul06 at fit.stud.vutbr.com)
 * @brief Main program for Santa Clasu problem assignement
 * @version 0.1
 * @date 2021-04-23
 */

#define log_file "proj2.out"
#define MAX(x, y) (((x) > (y)) ? (x) : (y))

#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <signal.h>
#include <fcntl.h>
#include <semaphore.h>
#include <errno.h>
#include <sys/mman.h>

typedef struct _ParamsType {
    int NE; // pocet skritku 0<NE<1000 - NElves
    int NR; // poset sobu 0<NR<20 - NReindeers
    int TE; // max doba v [ms] trvani samostatne prace skritka 0<=TE<=1000
    int TR; // max doba v [ms] trvani sobi dovolene 0<=TR<=1000
} ParamsType;

sem_t *santa_sem = NULL;
sem_t *elf_sem = NULL;
sem_t *hitch_sem = NULL;
sem_t *print_sem = NULL;
sem_t *hitched_sem = NULL;

int *output_counter;
int *rnd_counter;
bool *ready_for_christmass;
int *elf_counter;
FILE *outstream_file;

sem_t *elves_counter_sem = NULL;
sem_t *output_counter_sem = NULL;
sem_t *rnd_counter_sem = NULL;

void increment(int* arg, int cond){
    (*arg)++;
    cond++;
}

/**
 * @brief 2nd order function for executing arbitrary function in critical section
 * 
 * @param sem semaphor guarding section
 * @param arg argument to arbitrary function
 * @param fun_ptr function to execute in critical section
 */
void protected_operation(sem_t* sem, int* arg, void (*fun_ptr)(int*, int), int cond) {
    sem_wait(sem);
    // critical section
    (*fun_ptr)(arg, cond);
    // leaving critical section
    sem_post(sem);
}

void log_event(char* message, int id) {
    sem_wait(print_sem);

    (*output_counter)++;
    if (id > 0)
        fprintf(outstream_file, message,*output_counter, id);
    else
        fprintf(outstream_file, message,*output_counter);

    sem_post(print_sem);
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


void decrement_3(int* arg, int cond){
    (*arg) -= 3;
    (*arg) = MAX(0, *arg);
    cond++;
    sem_post(elf_sem);
    sem_post(elf_sem);
    sem_post(elf_sem);
}

void increment_if_cond_alert_santa(int* arg, int cond){
    if (cond != *arg) {
        (*arg)++;
        if (cond == *arg) sem_post(santa_sem);
    }
    else
        return;
}


void santa_process_main(int deer_count) {
    log_event("%i: Santa: going to sleep\n", -1);
    sem_wait(santa_sem);
    while (!(*rnd_counter == deer_count)){
        log_event("%i: Santa: helping elves\n", -1);
        protected_operation(elf_sem, elf_counter, decrement_3, 0);
        log_event("%i: Santa: going to sleep\n", -1);
        sem_wait(santa_sem);
    }
    log_event("%i: Santa: closing workshop\n", -1);
    for (int i = 0; i < deer_count; i++){
        sem_post(hitch_sem);
        sem_wait(hitched_sem);
    }
    *ready_for_christmass = true;
    log_event("%i: Santa: Christmas started\n", -1);
    exit(0);
}

void elf_process_main(int id, long int delay) {
    srand(time(0) + getpid());
    log_event("%i: ELF %i: started\n", id);
    while (!(*ready_for_christmass)){
        usleep(rand_in_range(0, delay));
        log_event("%i: ELF %i: need help\n", id);
        if (*ready_for_christmass){
            log_event("%i: ELF %i: taking holidays\n", id);
            exit(0);
        }
        protected_operation(elves_counter_sem, elf_counter, increment_if_cond_alert_santa, 3);
        sem_wait(elf_sem);
    }
    log_event("%i: ELF %i: taking holidays\n", id);
    exit(0);
}

void raindeer_process_main(int id, long int delay, long int r_count) {
    srand(time(0) + getpid());
    log_event("%i: RD %i: rstarted\n", id);
    usleep(rand_in_range( (int) delay/2, delay));

    log_event("%i: RD %i: return home\n", id);
    protected_operation(rnd_counter_sem, rnd_counter, increment_if_cond_alert_santa, r_count);
    
    sem_wait(hitch_sem);
    log_event("%i: RD %i: get hitched\n", id);
    sem_post(hitched_sem);
    exit(0);
}




/**
 * @brief 
 * 
 * @param argc 
 * @param argv 
 * @param params 
 * @return int 
 */
bool loadParams(int argc, char** argv, ParamsType* params){
    if (argc < 5) // not enough args
        return false;
    
    #define strtol_checked(p, i) do {\
        errno = 0;\
        p = strtol(argv[i], NULL, 10);\
        if (errno != 0) return false;\
    } while(0);

    strtol_checked(params->NE, 1);
    strtol_checked(params->NR, 2);
    strtol_checked(params->TE, 3);
    strtol_checked(params->TR, 4);

    if ((params->NE <= 0 || params->NE >= 1000) ||
        (params->NR <= 0 || params->NR >= 20)   ||
        (params->TE <  0 || params->TE >  1000) ||
        (params->TR <  0 || params->TR >  1000))
        return false;
    
    return true;
}

/**
 * @brief kills processes under pids in pidarray
 * 
 * @param pidarray process ids to kill
 * @param n count of processes to kill
 */
void kill_pids(pid_t* pidarray, int n){
    for (int i = 0; i < n; i++)
            kill(pidarray[i], SIGTERM);
    
}

int main(int argc, char *argv[]) {
    // Argument tests
    ParamsType params;
    if (!loadParams(argc, argv, &params)) {
        fprintf(stderr, "ERROR: Invalid or insufficient arguments entered \n");
        return 1;
    }

    output_counter = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0 );
    *output_counter = 0;
    output_counter_sem = sem_open("xschul06_output_count_sem", O_CREAT, 0666, 0);
    rnd_counter = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0 );
    *rnd_counter = 0;
    rnd_counter_sem = sem_open("xschul06_raindeer_count_sem", O_CREAT, 0666, 0);
    ready_for_christmass = mmap(NULL, sizeof(bool), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0 );
    *ready_for_christmass = false;
    elf_counter = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0 );
    *elf_counter = 0;
    elves_counter_sem = sem_open("xschul06_elves_count_sem", O_CREAT, 0666, 0);

    hitch_sem = sem_open("xschul06_hitch_sem", O_CREAT, 0666, 0);
    print_sem = sem_open("xschul06_print_sem", O_CREAT, 0666, 0);
    hitched_sem = sem_open("xschul06_hitched_sem", O_CREAT, 0666, 0);
    santa_sem = sem_open("xschul06_santa_sem", O_CREAT, 0666, 0);
    elf_sem = sem_open("xschul06_elf_sem", O_CREAT, 0666, 0);

    // first call should be succesfull
    sem_post(print_sem);


    pid_t *elf_processes = malloc(sizeof(pid_t) * params.NE);
    pid_t *raindeer_processes = malloc(sizeof(pid_t) * params.NR);
    if (elf_processes == NULL || raindeer_processes == NULL) {
        fprintf(stderr, "ERROR: Error allocating space for process identifiers!\n");
        goto cleanup_section;
        return 1;
    }


    outstream_file = fopen(log_file, "w");
    pid_t santa_process = fork();
    if (santa_process == 0) santa_process_main(params.NR);
    else if (santa_process < 0){
        fprintf(stderr, "ERROR: Error creating santa_process!\n");
        goto cleanup_section;
        return 1;
    }

    // initialize elf processes
    for (int i = 0; i < params.NE; i++){
        elf_processes[i] = fork();
        if (elf_processes[i] == 0) elf_process_main(i+1, params.TE);
        else if (elf_processes[i] < 0){
            kill(santa_process, SIGTERM);
            if (i > 0)
                kill_pids(elf_processes, i+1);
            fprintf(stderr, "ERROR: Error creating elves!\n");
            goto cleanup_section;
            return 1;
        }
    }

    // initialize raindeer processes
    for (int i = 0; i < params.NR; i++){
        raindeer_processes[i] = fork();
        if (raindeer_processes[i] == 0) raindeer_process_main(i+1, params.TR, params.NR);
        else if (raindeer_processes[i] < 0){
            kill(santa_process, SIGTERM);
            kill_pids(elf_processes, params.NE);
            if (i > 0)
                kill_pids(raindeer_processes, i+1);
            fprintf(stderr, "ERROR: Error creating reindeers!\n");
            goto cleanup_section;
            return 1;
        }
    }

    cleanup_section:
    munmap(output_counter, sizeof(int));
    munmap(elf_counter, sizeof(int));
    munmap(rnd_counter, sizeof(int));
    munmap(ready_for_christmass, sizeof(bool));

    sem_close(santa_sem);
    sem_unlink("xschul06_santa_sem");
    sem_close(elf_sem);
    sem_unlink("xschul06_elf_sem");
    sem_close(hitch_sem);
    sem_unlink("xschul06_hitch_sem");
    sem_close(print_sem);
    sem_unlink("xschul06_print_sem");
    sem_close(hitched_sem);
    sem_unlink("xschul06_hitched_sem");

    
    fclose(outstream_file);
    return 0;
}
