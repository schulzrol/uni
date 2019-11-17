#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h> // bool type
#include <ctype.h> // tolower

// due to minimal project requirements
#define MAXCONTACTS 42
#define MAXLINELEN 100
#define NOT_FOUND_STR "Not found\n"

// returns size of static array
#define ARRLEN(a) sizeof(a)/sizeof(*a)

typedef struct _contact{
    char phoneNum[MAXLINELEN + 1]; // + 1 because of NUL for string termination
    char fullName[MAXLINELEN + 1];
    bool toPrint;
} contact;

/*  Initalize what what each digit of number (argv[1]) can represent:
   0 (+,0)
   1 (1)
   2 (a,b,c,2)
   3 (d,e,f,3)
   4 (g,h,i,4)
   5 (j,k,l,5)
   6 (m,n,o,6)
   7 (p,q,r,s,7)
   8 (t,u,v,8)
   9 (w,x,y,z,9)*
 */
char* digitRep[10] = {"+0", "1", "abc2", "def3", "ghi4", "jkl5", "mno6", "pqrs7", "tuv8", "wxyz9"};

/* my_readLine
 * @s: buffer into which will the be line saved. Should be capable to hold at
 *     least @size_s + terminating NUL byte.
 * @size_s : length of @s
 * @readbytes: used as output for user, how many bytes read into @s
 *
 * - Reads from stdin until encounters either newline byte or EOF byte, in which
 *   case returns 0 and @readbytes is set to the number of bytes read, or until
 *   line length exceeds @size_s, in which case returns -1.
 *   The bytes read are stored in @s. 
 *
 *   When line length exceeds @size_s, no more is read, @s is failsafe NUL 
 *   terminated at @size_s-1.
 */
int my_readLine(char* s, size_t size_s, size_t* readbytes){
    if (!s || size_s == 0)
        return 0;

    *readbytes = 0;

    for(size_t i = 0; i < size_s; i++){
        int  c = getchar();
        if(c == EOF || c == '\n') {
           s[i] = '\0'; // add NUL byte
           return 0;
        }
        s[i] = (char) c;
        (*readbytes)++; // number of bytes read
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
    size_t readbytes;
    
    if (!newContact)
        return -1;

    /* Read information about new contact from stdin */
    // Initialize fullname
    retcode = my_readLine(newContact->fullName, ARRLEN(newContact->fullName), &readbytes);
    if (retcode < 0)
        return -2;
    if (readbytes > 0)
        nreadlines++;
        
    // Initialize phonenum
    retcode = my_readLine(newContact->phoneNum, ARRLEN(newContact->phoneNum), &readbytes);
    if (retcode < 0)
        return -2;
    if (readbytes > 0)
        nreadlines++;

    newContact->toPrint = true; // defaultly marked as suitable for printing - due to requirements

    return nreadlines;
}


/* printContact
 * @c: contact which details should be printed in csv format
 * 
 * Prints @c info on stdout. Should be used as abstraction.
 */
void printContact(contact c){
    if (c.fullName && c.phoneNum)
        printf("%s, %s\n", c.fullName, c.phoneNum);

    return;
}


/* printFilteredContacts - wrapper around printContact()
 * @contactBook: array of contacts to print
 * @ncontacts: number of contacts to print
 *
 * Prints out details about contacts marked to print to stdout stream.
 */
void printFilteredContacts(contact* contactBook, size_t ncontacts){
    if (!contactBook)
        return;

    for (size_t i = 0; i < ncontacts; i++) {
        if (contactBook[i].toPrint)
            printContact(contactBook[i]);
    }

    return;
}

/* filter_neprerusene
 * @filter: string containing the filter
 * @contactBook: contacts through which to filter
 * @ncontacts: number of contacts to filter
 *
 * Return values;
 * >=0 = number of succesfully filtered contacts
 *  -1 = general issue
 *  -2 = input filter is NaN
 */
int filter_neprerusene(char* filter, contact* contactBook, int ncontacts){
    if(!filter || !contactBook)
        return -1;
    int filtered = 0;
    size_t filterlen = strlen(filter);

    for(int cbi = 0; cbi < ncontacts; cbi++){ // go through @ncontacts contacts
        contact* currContact = &contactBook[cbi]; // current contact
        size_t rewindi = 0;
        size_t match = 0;

        currContact->toPrint = false; // defaultly doesnt match anything

        // search through phoneNum until either at the end or found a match
        for (size_t stri = 0; stri < strlen(currContact->phoneNum) && match != filterlen; stri++){
                int dri = char2dec(filter[match]); // digit representation index
                if (strchr(digitRep[dri], currContact->phoneNum[stri]) != NULL) {
                    match++;
                    if (match == 1) // remember the index to jump back in case filter doesnt pass
                        rewindi = stri;
                }
                else {
                    if (match > 0) // if filter didnt pass, jump back
                        stri = rewindi;
                    match = 0;
                }
        }
        
        // increment the filtered counter
        if (match == filterlen){
            currContact->toPrint = true;
            filtered++;
            continue;
        }

        match = 0;
        rewindi = 0;
        // search through fullName  until either at the end or found a match
        for (size_t stri = 0; stri < strlen(currContact->fullName) && match != filterlen; stri++){
                int dri = char2dec(filter[match]); // digit representation index
                if (strchr(digitRep[dri], tolower(currContact->fullName[stri])) != NULL) {
                    match++;
                    if (match == 1) // remember the index to jump back in case filter doesnt pass
                        rewindi = stri;
                }
                else {
                    if (match > 0) // if filter didnt pass, jump back
                        stri = rewindi;
                    match = 0;
                }
        }
        
        // increment the filtered counter
        if (match == filterlen){
            currContact->toPrint = true;
            filtered++;
        }

    }// end of contacts
    return filtered;
}

int main(int argc, char *argv[]){
    contact contactBook[MAXCONTACTS];
    int ncontacts = 0;
    int retcode;

    if (argc > 2) {
        printf("%s: Too many arguments!\n", argv[0]);
        return 1;
    }

    // Read paired info about contacts from stdin
    while ((retcode = readNewContact(&contactBook[ncontacts])) == 2) {
        //printf("%s: %s\n", contactBook[ncontacts].fullName, contactBook[ncontacts].phoneNum);
        ncontacts++;
    }
    // Detect anomalies (TODO: for sure exists better term than anomalies) when done reading
    if (retcode < 0) {
        if (retcode == -2)
            printf("%s: Line too long!\n", argv[0]);
        return 1;
    }
    if (retcode > 0) {
        printf("%s: Unpaired contact info on %d. contact!\n", argv[0], ncontacts + 1);
        return 1;
    }

    int nfiltered = 0;
    if (argc == 2){
        nfiltered = filter_neprerusene(argv[1], contactBook, ncontacts);
        // detect errors
        if (nfiltered < 0) {
            if (nfiltered == -2)
                printf("%s: Invalid input \"%s\" is NaN!\n", argv[0], argv[1]);
            return 1;
        }
        if (nfiltered == 0)
            printf(NOT_FOUND_STR);
    }

    printFilteredContacts(contactBook, ncontacts);

    return 0;
}
