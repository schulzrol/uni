#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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
#define MOJE_SKUPINA F

int main () {
    // vstup
    double U1 = skupiny[MOJE_SKUPINA][U1_id];
    double U2 = skupiny[MOJE_SKUPINA][U2_id];
    double R1 = skupiny[MOJE_SKUPINA][R1_id];
    double R2 = skupiny[MOJE_SKUPINA][R2_id];
    double R3 = skupiny[MOJE_SKUPINA][R3_id];
    double R4 = skupiny[MOJE_SKUPINA][R4_id];
    double R5 = skupiny[MOJE_SKUPINA][R5_id];
    double R6 = skupiny[MOJE_SKUPINA][R6_id];
    double R7 = skupiny[MOJE_SKUPINA][R7_id];
    double R8 = skupiny[MOJE_SKUPINA][R8_id];

    // secteni zdroju
    double U12 = U1 + U2;
    printf("U12 = %lf [V]\n", U12);

    // trojuhelnik -> hvezda
    double RA = (R1 * R2) / (R1 + R2 + R3);
    double RB = (R1 * R3) / (R1 + R2 + R3);
    double RC = (R2 * R3) / (R1 + R2 + R3);

    double RC6 = RC + R6;
    double RB4 = RB + R4;
    double RB45 = RB4 + R5;
    double RB45C6 = (RB45 * RC6) / (RB45 + RC6);
    double R78 = (R7 * R8) / (R7 + R8);
    double Rekv = RA + RB45C6 + R78;
    double I = U12/Rekv;
    double URB45C6 = I * RB45C6;
    double IRB45 = URB45C6 / RB45;
    double UR5 = IRB45 * R5;
    double IR5 = IRB45;

    printf("Vysledek:\n");
    printf("UR5 = %lf [V]\n IR5 = %lf[A]\n",UR5,IR5);
return 0;
}

