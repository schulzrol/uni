/******************************************************************************
 * Laborator 01 - Zaklady pocitacove grafiky - IZG
 * ihulik@fit.vutbr.cz
 *
 * $Id: $
 * 
 * Popis: Hlavicky funkci pro funkce studentu
 *
 * Opravy a modifikace:
 * - ibobak@fit.vutbr.cz, orderedDithering
 */

#include "student.h"
#include "globals.h"

#include <time.h>

const int M[] = {
        0, 204, 51, 255,
        68, 136, 187, 119,
        34, 238, 17, 221,
        170, 102, 153, 85
};

const int M_SIDE = 4;

/******************************************************************************
 ******************************************************************************
 Funkce vraci pixel z pozice x, y. Je nutne hlidat frame_bufferu, pokud 
 je dana souradnice mimo hranice, funkce vraci barvu (0, 0, 0).
 Ukol za 0.25 bodu */
S_RGBA getPixel(int x, int y)
{
    if ((x >= 0 && x < width) && (y >= 0 && y < height)) {
        return frame_buffer[(y*width) + x];
    }

    return COLOR_BLACK; //vraci barvu (0, 0, 0)
}
/******************************************************************************
 ******************************************************************************
 Funkce vlozi pixel na pozici x, y. Je nutne hlidat frame_bufferu, pokud
 je dana souradnice mimo hranice, funkce neprovadi zadnou zmenu.
 Ukol za 0.25 bodu */
void putPixel(int x, int y, S_RGBA color)
{
    if ((x >= 0 && x < width) && (y >= 0 && y < height)) {
        frame_buffer[(y*width) + x] = color;
    }
}
/******************************************************************************
 ******************************************************************************
 Funkce prevadi obrazek na odstiny sedi. Vyuziva funkce GetPixel a PutPixel.
 Ukol za 0.5 bodu */
void grayScale()
{
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            S_RGBA pixel = getPixel(x, y);
            // grayscale color
            unsigned char gsc = ROUND(pixel.red*0.299) + ROUND(pixel.green*0.587) + ROUND(pixel.blue*0.114);
            // znovupouziti nacteneho pixelu
            pixel.red = gsc;
            pixel.green = gsc;
            pixel.blue = gsc;
            putPixel(x, y, pixel);
        }
    }
}

/******************************************************************************
 ******************************************************************************
 Funkce prevadi obrazek na cernobily pomoci algoritmu maticoveho rozptyleni.
 Ukol za 1 bod */

void orderedDithering()
{
    /* vychazi se z sedotonoveho */
    grayScale();

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            S_RGBA pixel = getPixel(x, y);

            int i = x % M_SIDE;
            int j = y % M_SIDE;
            int mat_offset = (i * M_SIDE) + j;

            if (pixel.red > M[mat_offset]) {
                putPixel(x, y, COLOR_WHITE);
            } else {
                putPixel(x, y, COLOR_BLACK);
            }
        }
    }
}

/******************************************************************************
 ******************************************************************************
 Funkce prevadi obrazek na cernobily pomoci algoritmu distribuce chyby.
 Ukol za 1 bod */
// :precondition: framebuffer je grayscale
// helper k error_distribution()
void applyError(int x, int y, int error){
    S_RGBA pixel = getPixel(x ,y);
    // error corrected pixel value - muzu pouzit jen red, uz je to gs
    int errcpv = pixel.red + error;

    // saturated
    if (errcpv >= 255) {
        putPixel(x, y, COLOR_WHITE);
    }
        // empty as heck
    else if (errcpv <= 0) {
        putPixel(x, y, COLOR_BLACK);
    }
        // in bounds
    else {
        pixel.red = (unsigned char) errcpv;
        pixel.green = (unsigned char) errcpv;
        pixel.blue = (unsigned char) errcpv;
        putPixel(x, y, pixel);
    }
}

void errorDistribution()
{
    grayScale();
    unsigned char T = 127;

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            S_RGBA pixel = getPixel(x, y);
            //gray scale color pixel
            S_RGBA gscp;

            if (pixel.red > T) {
                gscp = COLOR_WHITE;
            } else {
                gscp = COLOR_BLACK;
            }
            putPixel(x, y, gscp);

            // chyba rozdil naprahovane barvy od puvodniho pixelu
            int error = pixel.red - gscp.red;

            // napravo od soucasneho pixelu
            applyError(x+1, y, ROUND(error*(3.0/8.0)));
            // pod soucasnym pixelem
            applyError(x, y+1, ROUND(error*(3.0/8.0)));
            // napravo dole od soucasneho pixelu
            applyError(x+1, y+1, ROUND(error*(2.0/8.0)));
        }
    }
}


/******************************************************************************
 ******************************************************************************
 Funkce prevadi obrazek na cernobily pomoci metody prahovani.
 Demonstracni funkce */
void thresholding(int Threshold)
{
    /* Prevedeme obrazek na grayscale */
    grayScale();

    /* Projdeme vsechny pixely obrazku */
    for (int y = 0; y < height; ++y)
        for (int x = 0; x < width; ++x)
        {
            /* Nacteme soucasnou barvu */
            S_RGBA color = getPixel(x, y);

            /* Porovname hodnotu cervene barevne slozky s prahem.
               Muzeme vyuzit jakoukoli slozku (R, G, B), protoze
               obrazek je sedotonovy, takze R=G=B */
            if (color.red > Threshold)
                putPixel(x, y, COLOR_WHITE);
            else
                putPixel(x, y, COLOR_BLACK);
        }
}

/******************************************************************************
 ******************************************************************************
 Funkce prevadi obrazek na cernobily pomoci nahodneho rozptyleni.
 Vyuziva funkce GetPixel, PutPixel a GrayScale.
 Demonstracni funkce. */
void randomDithering()
{
    /* Prevedeme obrazek na grayscale */
    grayScale();

    /* Inicializace generatoru pseudonahodnych cisel */
    srand((unsigned int)time(NULL));

    /* Projdeme vsechny pixely obrazku */
    for (int y = 0; y < height; ++y)
        for (int x = 0; x < width; ++x)
        {
            /* Nacteme soucasnou barvu */
            S_RGBA color = getPixel(x, y);

            /* Porovname hodnotu cervene barevne slozky s nahodnym prahem.
               Muzeme vyuzit jakoukoli slozku (R, G, B), protoze
               obrazek je sedotonovy, takze R=G=B */
            if (color.red > rand()%255)
            {
                putPixel(x, y, COLOR_WHITE);
            }
            else
                putPixel(x, y, COLOR_BLACK);
        }
}
/*****************************************************************************/
/*****************************************************************************/
