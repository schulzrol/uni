/**
 * Kostra pro cviceni operaci nad jednosmerne vazanym seznamem.
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
 
/**
 * struktura Object
 */
typedef struct {
  int id;
  char *name;
} Object;
 
/**
 * Polozka seznamu
 */
typedef struct item Item;
struct item {
  Object data;
  Item *next;
};
 
/**
 * Typ seznam
 */
typedef struct {
  Item *first;
} List;
 
/**
 * Inicializace seznamu. Vytvori prazdny seznam.
 */
List list_ctor()
{
	List newlist;
	newlist.first = NULL;
 	return newlist;
}
 
/**
 * Inicializace polozky seznamu. Z objektu vytvori polozku bez naslednika.
 */
Item *item_ctor(Object data)
{
	Item* newItem = malloc(sizeof(*newItem));
	if (newItem == NULL)
		return NULL;
	newItem->data = data;
	newItem->next = NULL;
	return newItem;
}
 
 
/**
 * Vlozi polozku na zacatek seznamu.
 */
void list_insert_first(List *list, Item *i)
{
 	if (list == NULL || i == NULL)
		return;

	// list->first -- i -- list->first->next
	Item* tempfirst = list->first;
	i->next = tempfirst;
	list->first = i;
	return;
}
 
/**
 * Vrati true, pokud je seznam prazdny.
 */
bool list_empty(List *list)
{
	return (list == NULL || list->first == NULL) ? true: false;
}
 
/**
 * Odstrani prvni prvek seznamu, pokud je.
 */
void list_delete_first(List *list)
{
	if (list_empty(list))
		return;
	Item* tempfirst = list->first;
	list->first = tempfirst->next;
	free(tempfirst);
	return;
}
 
/**
 * Vrati pocet polozek seznamu.
 */
unsigned list_count(List *list)
{
	unsigned icnt = 0;
	if (list == NULL)
		return icnt;
	Item* itemi = list->first;
	while (itemi != NULL) {
		icnt++;
		itemi = itemi->next;
	}
	return icnt;
}
 
 
/**
 * Najde polozku seznamu s nejmensim identifikatorem. Vraci NULL, pokud je
 * seznam prazdny.
 */
Item *list_find_minid(List *list)
{
	if (list_empty(list))
		return NULL;

	Item* itemi = list->first;
	Item* found = itemi;
	while (itemi != NULL) {
		if (found->data.id > itemi->data.id)
			found = itemi;
		itemi = itemi->next;
	}
	
	return found;
}
 
/**
 * Najde polozku seznamu s odpovidajicim jmenem objektu. Vraci NULL, pokud
 * takova polozka v seznamu neni.
 */
Item *list_find_name(List *list, char *name)
{
	if (list == NULL || name == NULL)
		return NULL;

	Item* found = list->first;
	while (found != NULL) {
		// compare name in list to given name
		if (strcmp(found->data.name, name) == 0)
			return found;
		found = found->next;
	}
	
	return NULL;
}
 
/**
 * Uvolneni seznamu.
 */
void list_dtor(List *list)
{
	while (!list_empty(list))
		list_delete_first(list);

	return;
}
 
int main()
{
    printf("list_ctor...\n");
    List list = list_ctor();
 
    printf("list_empty...\n");
    printf("Seznam prazdny: %s\n", list_empty(&list) ? "ano" : "ne");
 
    Item *item;
 
    Object o1 = {42, "Honza"};
    printf("item_ctor...\n");
    item = item_ctor(o1);
    printf("list_insert_first...\n");
    list_insert_first(&list, item);
 
    printf("Seznam prazdny: %s\n", list_empty(&list) ? "ano" : "ne");
    printf("list_count...\n");
    printf("Pocet prvku v seznamu: %d\n", list_count(&list));
 
    Object o2 = {2, "Malem"};
    item = item_ctor(o2);
    printf("list_insert_first...\n");
    list_insert_first(&list, item);
 
    Object o3 = {0, "Kralem"};
    item = item_ctor(o3);
    printf("list_insert_first...\n");
    list_insert_first(&list, item);
 
    printf("Pocet prvku v seznamu: %d\n", list_count(&list));
 
    printf("Odstraneni prvniho prvku ze seznamu \n");
    list_delete_first(&list);
    printf("Pocet prvku v seznamu: %d\n", list_count(&list));
 
 
    // opetovne vlozeni objektu o1		
    item = item_ctor(o1);
    printf("list_insert_first...\n");
    list_insert_first(&list, item);
 
    printf("list_find_minid...\n");
    item = list_find_minid(&list);
    if (item != NULL) {
        printf("Polozka s nejmensim identifikatorem: {%d, \"%s\"}\n",
            item->data.id, item->data.name);
	}    
	else
            printf("Polozka s nejmensim identifikatorem nenalezena\n");
 
    printf("list_find_name...\n");
    char* name = "Honza";
    item = list_find_name(&list, name);
    if (item != NULL) {
             printf("Polozka s daty %s nalezena\n", name);
	}    
	else
             printf("Polozka s daty %s nenalezena.\n",name);
 
    printf("list_dtor...\n");
    list_dtor(&list);
    printf("Seznam prazdny: %s\n", list_empty(&list) ? "ano" : "ne");
 
    return 0;
}
