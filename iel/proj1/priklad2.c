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
     U_id,
    R1_id,
    R2_id,
    R3_id,
    R4_id,
    R5_id,
    R6_id,
    VSTUP_CNT,
} vstup;

double skupiny[SKUP_CNT][VSTUP_CNT] = {
    // U  R1  R2  R3  R4  R5  R6
    { 50,100,525,620,210,530,100}, // A
    {100, 50,310,610,220,570,200}, // B
    {200, 70,220,630,240,450,300}, // C
    {150,200,200,660,200,550,400}, // D
    {250,150,335,625,245,600,150}, // E
    {130,180,350,600,195,650,250}, // F
    {180,250,315,615,180,460,350}, // G
    {220,190,360,580,205,560,180}, // H
};
#define MOJE_SKUPINA C

int main () {
    // vstup ze zadani
    _( U, skupiny[MOJE_SKUPINA][U_id], "V");
    _(R1, skupiny[MOJE_SKUPINA][R1_id], "Ω");
    _(R2, skupiny[MOJE_SKUPINA][R2_id], "Ω");
    _(R3, skupiny[MOJE_SKUPINA][R3_id], "Ω");
    _(R4, skupiny[MOJE_SKUPINA][R4_id], "Ω");
    _(R5, skupiny[MOJE_SKUPINA][R5_id], "Ω");
    _(R6, skupiny[MOJE_SKUPINA][R6_id], "Ω");

    // predpoklad
    _(R12, R1 + R2, "Ω");
    _(R123, (R12 * R3)/(R12 + R3), "Ω");
    _(R1234, R123 + R4, "Ω");
    _(R12345, (R1234 * R5)/(R1234 + R5), "Ω");
    _(Ri, R12345, "Ω");


    // vypocet Proudu a Napeti
    //1
    _(RR12, R1 + R2,"Ω");
    _(RR45, R4 + R5, "Ω");
    _(RR345, (RR45*R3)/(RR45+R3), "Ω");
    _(RR12345, RR12 + RR345, "Ω");
    //2
    _(I, U/RR12345, "A");
    //3
    _(I345, I, "A");
    _(U345, I * RR345, "V");
    //4
    _(U45, U345, "V");
    _(I45, U345/RR45, "A");
    //5
    _(I5, I45, "A");
    _(UR5, I45 * R5, "V");
    _(Ui, UR5, "V");

    //Vysledek
    printf("Vysledek:\n");
    _(IR6, Ui/(Ri + R6), "A");
    _(UR6, R6 * IR6, "V");

    return 0;
}

