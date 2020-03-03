#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h> //getopt()
#include <errno.h>

#define RET_INVALID_MAZE -1
#define RET_VALID_MAZE 0

#define ROWDELTA_I 0
#define COLDELTA_I 1
#define CROSSED_I  2

#define RIGHTHAND_RULE -1
#define LEFTHAND_RULE   1

#define INVARGERR_STR "Invalid arguments, use --help for guidance"
#define PRINT_INVARGERR_AND_RETURN {\
                                  fprintf(stderr, "%s: " INVARGERR_STR "\n", argv[0]);\
                                  return 1;\
                                 }
typedef struct {
  int rows;
  int cols;
  unsigned char *cells;
} Map;

typedef enum {
    B_LEFT  = 0,
    B_RIGHT = 1 << 0,
    B_MID   = 1 << 1,
    B_COUNT,
} borderType;
#define BORDERS_IN_TRIANGLE 3

typedef struct {
    int rowdelta;
    int coldelta;
    int crossed;
} moveT;

// [triangle orientation][border] = [rowdelta, coldelta, crossed in new]
// [2] because triangle can be either UP or DOWN
// [BORDERS_IN_TRIANGLE].. pretty self explanatory
// [3] since we want to store three move informations
int movesTable[2][BORDERS_IN_TRIANGLE][3] = {
    { // points up
     { 0, -1, B_RIGHT}, // go left border
     { 0,  1, B_LEFT},  // go right border
     { 1,  0, B_MID}    // go middle border
    },
    { // points down
     { 0, -1, B_RIGHT}, // go left border
     { 0,  1, B_LEFT},  // go right border
     {-1,  0, B_MID}    // go middle border
    }
};

bool outOfBounds(Map* map, int r, int c){
    return (map == NULL) ? false : (r < 0 || r >= map->rows || c < 0 || c >= map->cols);
}

// returns 1 if triangle points down, 0 if points up
int triangleRot(int row, int col){
    return ((row+col) & 1) ? 1: 0;
}

Map* newMap(int rows, int cols){
    //allocate map object
    Map* retMap = malloc(sizeof(*retMap));
    if (retMap == NULL)
        return NULL;

    
    //allocate cells
    //TODO: add proper name of this type of 2D array
    int size = rows * cols;
    retMap->cells = malloc(sizeof(*retMap->cells) * size);
    if (retMap->cells == NULL && size != 0) {
        free(retMap);
        return NULL;
    }
    
    retMap->rows = rows;
    retMap->cols = cols;

    return retMap;
}

void freeMap(Map* map){
    if (map != NULL){
        free(map->cells);
    }
    free(map);

    return;
}

int setCell(Map* map, int row, int col, unsigned char value){
    if (map == NULL || map->cells == NULL || row >= map->rows || col >= map->cols)
        return -1;

    int index = (map->cols * row) + col;
    map->cells[index] = value;

    return 0;
}

unsigned char*
getCell(Map* map, int row, int col){
    if (map == NULL || map->cells == NULL || outOfBounds(map, row, col))
        return NULL;

    int index = (map->cols * row) + col;
    return &map->cells[index];
}

Map* loadMapfromFILE(FILE* fptr, int* err){
    if (fptr == NULL)
        return NULL;

    bool reterr = true;
    if (err == NULL)
        reterr = false;

    int rows, cols;
    if (fscanf(fptr, "%d %d", &rows, &cols) != 2) {
        if (reterr) *err = RET_INVALID_MAZE;
        return NULL;
    }
    
    Map* retMap = newMap(rows, cols);
    if (retMap == NULL)
        return NULL;

    for (int rowi = 0; rowi < rows; rowi++) {
        for (int coli = 0; coli < cols; coli++){
            unsigned char temp;
            // check read count
            if (fscanf(fptr, "%hhu", &temp) == 1){
                // check if is valid number of borders
                if (temp <= 7) {
                    setCell(retMap, rowi, coli, temp);
                    continue;
                }
            }

            if (reterr) *err = RET_INVALID_MAZE;
            freeMap(retMap);
            return NULL;
        }
    }

    return retMap;
}

bool isborder(Map *map, int r, int c, int border){
    unsigned char* cell = getCell(map, r, c);
    if (cell == NULL)
        return false;

    return (*cell & border) ? true : false;
}

void printMap(Map* map){
    if (map == NULL || map->cells == NULL)
        return;

    for (int rowi = 0; rowi < map->rows; rowi++) {
        for (int coli = 0; coli < map->cols; coli++){
            printf("%hhu ", *getCell(map, rowi, coli));
        }
        printf("\n");
    }
    return;
}

//TODO
bool isValidMap(Map* map){
    if (map == NULL)
        return false;

    return true;
}


int start_border(Map* map, int r, int c, int leftright){
    int triedBorders = 0;
    int tryborder;

    while (triedBorders <= BORDERS_IN_TRIANGLE) { /* Turn and look for an open edge. */
        tryborder = (tryborder+leftright)%3;
        if (isborder(map, r, c, tryborder)) {
            break;
        } 
    } 
    if (triedBorders == BORDERS_IN_TRIANGLE)
        return -1;
    return 0; // just a placeholder
}

// Returns:
//  0 found exit
//  -1 not found exit
int solveMaze(Map* map, int row, int col, int enterBorder, int handturn) {
    //int lastcrossed = enterBorder;
    //checks
    if (map == NULL || outOfBounds(map, row, col) || isborder(map, row, col, enterBorder))
        return -1;

    // while not escaped maze
    while (!outOfBounds(map, row, col)) {
        printf("%d %d\n", row+1, col+1); //required out format

        int moveBorder = start_border(map, row, col, handturn);

        int* move = movesTable[triangleRot(row, col)][moveBorder];
        row += move[0];        /* After looking up the move, update the */
        col += move[1];        /*  cell position and the edge number. */
     //   lastcrossed = move[2];
    }
    return 0;
}

void printHelp(){
    const char* helpString =
        "usage: [--help, --test, --rpath, --lpath] filename\n"
        "\t--help - prints this usage guide\n"
        "\t--test - checks whether filename contains valid map\n"
        "\t--rpath - sets the right hand rule for maze solving\n"
        "\t--lpath - sets the left hand rule for maze solving\n"
        "\tfilename - necesarry when using --test, --rpath or --lpath\n";
    printf(helpString);
    return;
}

/* pedantic_strtod
 * @nptr:  lookup manual "man 3 strtod", string containing number
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
ezstrtod(const char* nptr,
                int* saven){

    char *endptr = NULL;
    int retn = strtod(nptr, &endptr);

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

int main(int argc, char* argv[]){
    // filename is always the last one - due to examples
    const char* mapfname = argv[argc-1];
    FILE* mapfp = fopen(mapfname, "r");
    int startrow, startcol;
    int handturn = 0;
    bool helpmode = false;
    bool testmode = false;
    bool solvemode = false;

    //handle arguments due to examples
    switch (argc){
    case 2: // only --help, expects <>
        if (!strcmp(argv[1], "--help")) {
            helpmode = true;
            break;
        }
        PRINT_INVARGERR_AND_RETURN;

    case 3: // only --test, expects <filename>
        if (!strcmp(argv[1], "--test")) {
            testmode = true;
            break;
        }
        PRINT_INVARGERR_AND_RETURN;
    case 5: // maze solving mode, expects <mode> <R> <C> <filename>
        if (!strcmp(argv[1], "--lpath")) {
            handturn = LEFTHAND_RULE;
            solvemode = true;
            break;
        }
        else if (strcmp(argv[1], "--rpath")) {
            handturn = RIGHTHAND_RULE;
            solvemode = true;
            break;
        }
        PRINT_INVARGERR_AND_RETURN;

    default:
        PRINT_INVARGERR_AND_RETURN;
    }

    //werk
    if (helpmode) {
        printHelp();
        return 0;
    }

    if (testmode) {
        if (isValidMap(loadMapfromFILE(mapfp, NULL)))
            printf("Valid\n");
        else
            printf("Invalid\n");
        return 0;
    }

    if (solvemode) {
        Map* map = loadMapfromFILE(mapfp, NULL);
        if (map == NULL)
            return -1;

        if (!ezstrtod(argv[2], &startrow) || !ezstrtod(argv[3], &startcol))

        solveMaze(map, startrow, startcol, 2, handturn); // 2 just a placeholder

        printMap(map);

        freeMap(map);
    }

    return 0;
}
