#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "helpermacros.h"

typedef enum {
    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    SKUP_CNT
} skupina;

typedef enum {
    U1_id,
    U2_id,
    R1_id,
    R2_id,
    R3_id,
    R4_id,
    R5_id,
    R6_id,
    R7_id,
    R8_id,
    VSTUP_CNT,
} vstup;

double skupiny[SKUP_CNT][VSTUP_CNT] = {
    //U1  U2  R1  R2  R3  R4  R5  R6  R7  R8
    { 80,120,350,650,410,130,360,750,310,190}, // A
    { 95,115,650,730,340,330,410,830,340,220}, // B
    {100, 80,450,810,190,220,220,720,260,180}, // C
    {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0}, // D
    {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0}, // E
    {125, 65,510,500,550,250,300,800,330,250}, // F
    {103, 60,380,420,330,440,450,650,410,275}, // G
    {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0}, // H
};
#define MOJE_SKUPINA A

int main () {
    // vstup
    _(U1, skupiny[MOJE_SKUPINA][U1_id], "V");
    _(U2, skupiny[MOJE_SKUPINA][U2_id], "V");
    _(R1, skupiny[MOJE_SKUPINA][R1_id], "omega");
    _(R2, skupiny[MOJE_SKUPINA][R2_id], "omega");
    _(R3, skupiny[MOJE_SKUPINA][R3_id], "omega");
    _(R4, skupiny[MOJE_SKUPINA][R4_id], "omega");
    _(R5, skupiny[MOJE_SKUPINA][R5_id], "omega");
    _(R6, skupiny[MOJE_SKUPINA][R6_id], "omega");
    _(R7, skupiny[MOJE_SKUPINA][R7_id], "omega");
    _(R8, skupiny[MOJE_SKUPINA][R8_id], "omega");

    // secteni zdroju
    _(U12, U1 + U2, "V");

    // trojuhelnik -> hvezda
    _(RA, (R1 * R2) / (R1 + R2 + R3), "omega");
    _(RB, (R1 * R3) / (R1 + R2 + R3), "omega");
    _(RC, (R2 * R3) / (R1 + R2 + R3), "omega");

    _(RC6, RC + R6, "omega");
    _(RB4, RB + R4, "omega");
    _(RB45, RB4 + R5, "omega");
    _(RB45C6, (RB45 * RC6) / (RB45 + RC6), "omega");
    _(R78, (R7 * R8) / (R7 + R8), "omega");
    _(Rekv, RA + RB45C6 + R78, "omega");
    _(I, U12/Rekv, "A");
    _(URB45C6, I * RB45C6, "V");
    _(IRB45, URB45C6 / RB45, "A");
    _(IRC6, URB45C6/RC6, "A");

    printf("Vysledek:\n");
    _(UR5, IRB45 * R5,"V");
    _(IR5, IRB45, "A");

    return 0;
}
