/* Roland Schulz's second IZP project
 *
 * compile using: 
 * gcc -std=c99 -Wall -Wextra -Werror proj2.c -lm -o proj2
 */

#include <stdio.h>
#include <stdlib.h> // strtod
#include <math.h>  // INFINITY
#include <string.h>
#include <stdbool.h>
#include <errno.h>

#define INVARGERR_STR "invalid arguments"
#define PRINT_INVARGERR_AND_RETURN {\
                                  fprintf(stderr, "%s: " INVARGERR_STR "\n", "error");\
                                  return 1;\
                                 }

// turn on debug info by defining DEBUG
//#define DEBUG

extern int errno; // error handling

const double I0 =    1e-12; // [A]
const double UT = 0.0258563; // [V]

// exp(Up/UT) == e^Up/UT 

/* pedantic_strtod
 * @nptr:  lookup manual "man 3 strtod"
 * @saven: Will contain the saved number, if passes all
 *         pedantic requirements listed below
 *
 * Returns true in case no {over,under}flows happen and @endptr
 * dereferenced value differs from string terminating byte ('\0')
 * in that case also saves the double string representation in @nptr 
 * to @saven.
 * Otherwise conversion was not succesfull and returns false.
 */
bool
pedantic_strtod(const char* nptr,
                double* saven){
    
    char *endptr = NULL;
    double retn = strtod(nptr, &endptr);
    
    // cases below are described in manual of strtod(3)
    // in case there is any {over,under}flows happening
    if (errno == ERANGE)
        return false;
    // if no conversion or there is some string part after the number
    if (endptr == nptr || *endptr != '\0')
        return false;

    *saven = retn;
    return true;
}

/* Reference circuit for diode()
 *             R       D
 * +3.3V o----/\/\----|>|---o 0V
 */

/* diode
 * @uo: voltage source
 * @r: resistance of resistor
 * @eps: allowed deviation
 *
 * Returns the operating voltage of diode using bisection method,
 * between 0.0 (due to req) and @u0.
 *
 * WARNING: Expects correct values for inputs! Should be checked
 *          before calling!
 */
double diode(double u0, double r, double eps){
    #ifdef DEBUG
    size_t cycles = 0;
    #endif
    // voltage level bounds
    double lowerbound = 0.0; 
    double upperbound = u0;
    
    double Up = (upperbound + lowerbound)/2; // pomoci bisekce
    double dosazena_presnost = fabs(upperbound - lowerbound);

    // bisection method of guessing Up
    // with protection against too small epsilon doubles cant handle it
    while (dosazena_presnost > eps) {
        Up = (upperbound + lowerbound)/2; // novy stred pomoci bisekce
        if (Up == upperbound || Up == lowerbound)
            break;
        double UR = u0 - Up;
        // pro kontrolu
        double IR = UR/r;
        double Ip = I0 * (exp(Up/UT) - 1);
        if (Ip > IR)
            upperbound = Up;
        else
            lowerbound = Up;

        dosazena_presnost = fabs(upperbound - lowerbound);
        // DEBUG
        #ifdef DEBUG
        printf("%g | Up=%g | Ip=%g | IR=%g\n", fabs(eps - dosazena_presnost), Up, Ip, IR);
        printf("upper=%g\nlower=%g\n", upperbound, lowerbound);
        printf("Ipresnost=%g\n", fabs(Ip - IR));
        printf("%lu\n", ++cycles);
        #endif
    }

    return Up;
}

int main(int argc, char *argv[]) { // input U0 R EPS
    double  U0;
    double   R;
    double EPS;

    if (argc != 4)
        PRINT_INVARGERR_AND_RETURN;

    // read the values from argv
    // U0
    if (pedantic_strtod(argv[1], &U0)) {
        // cannot have negative voltage due to requirements
        if (U0 < 0.0)
            PRINT_INVARGERR_AND_RETURN;
    }
    else
        PRINT_INVARGERR_AND_RETURN;
    
    // R
    if (pedantic_strtod(argv[2], &R)){
        // cant have negative resistance, or divide by zero
        if (R <= 0.0)
            PRINT_INVARGERR_AND_RETURN;
    }
    else
        PRINT_INVARGERR_AND_RETURN;
    
    // EPS
    if (pedantic_strtod(argv[3], &EPS)){
        // cant fulfill negative, zero, or -inf precision (inf precision is kinda nonsense but we can do it)
        if (isinf(EPS) == -1 || fpclassify(EPS) == FP_ZERO || EPS <= 0.0) // man 3 fpclassify
            PRINT_INVARGERR_AND_RETURN;
    }
    else
        PRINT_INVARGERR_AND_RETURN;

    // vypocet
    double Up = diode(U0, R, EPS);
    double Ip = I0 * (exp(Up/UT) - 1);

    printf("Up=%g V\n", Up);
    printf("Ip=%g A\n", Ip);
    return 0;
};
