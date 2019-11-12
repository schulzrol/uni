#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

// due to minimal project requirements
#define MAXCONTACTS 42
#define MAXLINELEN 100
#define NOT_FOUND_STR "Not found"

// returns size of static array
#define ARRLEN(a) sizeof(a)/sizeof(*a)

typedef struct _contact{
    char phoneNum[MAXLINELEN + 1]; // + 1 because of NUL for string termination
    char fullName[MAXLINELEN + 1];
    bool toPrint;
} contact;

/*  Initalize what what each digit of number (argv[1]) can represent:
   0 (+,0)
   1 ()
   2 (a,b,c,3)
   3 (d,e,f,3)
   4 (g,h,i,4)
   5 (j,k,l,5)
   6 (m,n,o,6)
   7 (p,q,r,s,7)
   8 (t,u,v,8)
   9 (w,x,y,z,9)*
 */
char* digitRep[10] = {"+0", "", "abc2", "def3", "ghi4", "jkl5", "mno6", "pqrs7", "tuv8", "wxyz9"};

/* my_readLine
 * @s: buffer into which will the be line saved. Should be capable to hold at
 *     least @size_s + terminating NUL byte.
 * @size_s : length of @s
 *
 * - Reads from stdin until encounters either newline byte, EOF byte, in which
 *   case returns the number of bytes read, or until line length exceeds @size_s,
 *   in which case returns negative value. The bytes read are stored in @s.
 *   When line length exceeds @size_s, no more is read, @s is failsafe NUL 
 *   terminated at @size_s-1.
 */
int my_readLine(char* s, int size_s){
    if (!s)
        return 0;

    for(int i = 0; i < size_s; i++){
        int  c = getchar();
        if(c == EOF || c == '\n') {
           s[i] = '\0'; // add NUL byte
           return i;    // number of bytes read
        }
        s[i] = (char) c;
    }
    
    s[size_s-1] = '\0'; // just in case someone ignores error value - rather end with NUL
    return -1; // Line too long
}

/* char2dec
 * @c: digit character, which will be converted to the value it represents
 *
 * -In case @c is a valid digit char representation ('0' to '9'), then this
 *  function returns the number the char represents, ie. char2dec('0') == 0.
 *  Otherwise returns negative value -1!
 */
int char2dec(char c){
    if (c >= '0' && c <= '9'){ // '0' <= c <= '9'
        return (int) c - '0';  // due to ascii arrangement
    }
    return -1;
}

/* readNewContact
 * @newContact: Contact variable to initialize.
 *
 * - Initializes new contact variable with structured input from stdin.
 *   Returns the number of succesfully read lines, otherwise one of following:
 *   -1: General error (invalid arguments)
 *   -2: Line too long
 */
int readNewContact(contact *newContact){
    int nreadlines = 0;
    int retcode;
    
    if (!newContact)
        return -1;

    /* Read information about new contact from stdin */
    // Initialize fullname
    retcode = my_readLine(newContact->fullName, ARRLEN(newContact->fullName));
    if (retcode < 0)
        return -2;
    if (retcode > 0)
        nreadlines++;
        
    // Initialize phonenum
    retcode = my_readLine(newContact->phoneNum, ARRLEN(newContact->phoneNum));
    if (retcode < 0)
        return -2;
    if (retcode > 0)
        nreadlines++;

    newContact->toPrint = true; // defaultly marked as suitable for printing

    return nreadlines;
}

/* printContacts
 * @contactBook: array of contacts to print
 * @ncontacts: number of contacts to print
 *
 * Prints details about contacts to stdout stream.
 *
 * Warning: Doesn't check whether contact details are valid strings or NULL
 */
void printContacts(contact* contactBook, size_t ncontacts){
    if (!contactBook)
        return;

    for (size_t i = 0; i < ncontacts; i++) {
        printf("%s, %s\n", contactBook[i].fullName, contactBook[i].phoneNum);
    }

    return;
}

int main(int argc, char *argv[]){
    contact contactBook[MAXCONTACTS];
    int ncontacts = 0;
    int retcode;

    if (argc > 2) {
        printf("%s: Too many arguments!\n", argv[0]);
        return 1;
    }

    printf("Contact Reading:\n");

    // Read paired info about contacts from stdin
    while ((retcode = readNewContact(&contactBook[ncontacts])) == 2) {
        printf("%s: %s\n", contactBook[ncontacts].fullName, contactBook[ncontacts].phoneNum);
        ncontacts++;
    }
    // Detect anomalies (TODO: for sure exists better term than anomalies) when done reading
    if (retcode == -2) 
        printf("%s: Line too long!\n", argv[0]);
    if (retcode > 0)
        printf("%s: Unpaired contact info on %d. contact!\n", argv[0], ncontacts + 1);

    printf("Contact Printing:\n");
    printContacts(contactBook, ncontacts);

    if (argc == 2)
    for (size_t i = 0; i < strlen(argv[1]); i++) {
        char c = argv[1][i];
        int d = char2dec(c);
        if (d >= 0)
            printf("\'%c\' == %d\n", c, char2dec(c));
        else
            printf("\'%c\' is NaN\n", c);
    }


    return 0;
}
