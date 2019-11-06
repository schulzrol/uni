#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#define DOPO 0
#define ODPO 1
 
struct time_s { int hour; int min; };
typedef struct oh {
  struct time_s start;
  struct time_s finish;
} OfficeHours; // ordinacni hodiny
 
// DoW = day of week
enum DoW { DOW_PO, DOW_UT, DOW_ST, DOW_CT, DOW_PA, DOW_SO, DOW_NE,
    DOW_COUNT };
 
// Nacte z otevreneho souboru ordinacni hodiny.
int fload_oh(FILE *f, OfficeHours oh[][2])
{
    for (int i = DOW_PO; i < DOW_COUNT; i++)
    {
        // nacteni dopolednich ordinacnich hodin
        if (fscanf(f, "%d:%d %d:%d", &oh[i][0].start.hour, &oh[i][0].start.min, &oh[i][0].finish.hour, &oh[i][0].finish.min) != 4)
            return -1;
        // nacteni odpolednich ordinacnich hodin
        if (fscanf(f, "%d:%d %d:%d", &oh[i][1].start.hour, &oh[i][1].start.min, &oh[i][1].finish.hour, &oh[i][1].finish.min) != 4)
            return -1;
    }
    return DOW_COUNT;
}
 
int load_oh(char *filename, OfficeHours oh[][2])
{
    FILE *f = fopen(filename,"r");
    if (f == NULL)
        return -1;
    int code = fload_oh(f, oh);
    fclose(f);
    return code;
}
 
// cviceni za domaci ukol
// is wrong
bool is_now_open(OfficeHours oh[][2])
{
    bool isOpen = false;

    time_t now = time(NULL);
    struct tm *datetime = localtime(&now);

    /*	Tabulka prevodu mezi nasim a standardem
     *	DNY NASE TM_X
     *   Po    0    1
     * 	 Ut    1    2
     * 	 St    2    3
     * 	 Ct    3    4
     * 	 Pa    4    5
     * 	 So    5    6
     * 	 Ne    6    0
     */

    int now_day = (datetime->tm_wday - 1)%DOW_COUNT;
    int now_hour = datetime->tm_hour;
    int now_min = datetime->tm_min;

    if ((now_hour >= oh[now_day][DOPO].start.hour && now_hour <= oh[now_day][DOPO].finish.hour) || 
		(now_hour >= oh[now_day][ODPO].start.hour && now_hour <= oh[now_day][ODPO].finish.hour)) {
		// matches hours
		// minute part is wrong - would trigger when 12:35 if oh is for example 11:45 - 12:30
		if ((now_minute >= oh[now_day][DOPO].start.minute && now_minute <= oh[now_day][DOPO].finish.minute) || 
			(now_minute >= oh[now_day][ODPO].start.minute && now_minute <= oh[now_day][ODPO].finish.minute)) {
			// matches both hours and day
			isOpen = true;
		}
	}

    return isOpen;
}
 
/**
 * Priklad vstupniho souboru hodiny_lekar.txt, -1:0 znamena, ze v danem
 * terminu neordinuje:
 * 7:00 11:30 13:00 16:30
 * -1:0 -1:0  13:00 16:30
 * 9:00 12:00 14:00 18:00
 * 9:00 12:00 13:00 14:00
 * 9:00 11:30 -1:0 -1:0
 */
 
int main()
{
    OfficeHours oh[DOW_COUNT][2]; // dva intervaly hodin v jednom dni
    // oh je pole sedmi (DOW_COUNT=7) poli, kde kazdy prvek je pole dvou struktur ordinacnich hodin.
 
    // nacteni dat ze souboru
    load_oh("hodiny_lekar.txt", oh);
 
    /* konstrukce lze zobecnit pomoci dat napriklad takto: */
    char *open_str[] = {"zavreno", "otevreno"};
    printf("Lekar ma ted %s.\n", open_str[is_now_open(oh)]);
 
    /* ale zde citelnejsi zapis je vsak tento: */
    if (is_now_open(oh))
        printf("Lekar ma ted otevreno.\n");
    else
        printf("Lekar ma ted zavreno.\n");
 
    return 0;
}
