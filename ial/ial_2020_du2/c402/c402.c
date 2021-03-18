
/* c402.c: ********************************************************************}
{* Téma: Nerekurzivní implementace operací nad BVS
**                                     Implementace: Petr Přikryl, prosinec 1994
**                                           Úpravy: Petr Přikryl, listopad 1997
**                                                     Petr Přikryl, květen 1998
**			  	                        Převod do jazyka C: Martin Tuček, srpen 2005
**                                         Úpravy: Bohuslav Křena, listopad 2009
**                                                 Karel Masařík, říjen 2013
**                                                 Radek Hranický 2014-2018
**
** S využitím dynamického přidělování paměti, implementujte NEREKURZIVNĚ
** následující operace nad binárním vyhledávacím stromem (předpona BT znamená
** Binary Tree a je u identifikátorů uvedena kvůli možné kolizi s ostatními
** příklady):
**
**     BTInit .......... inicializace stromu
**     BTInsert ........ nerekurzivní vložení nového uzlu do stromu
**     BTPreorder ...... nerekurzivní průchod typu pre-order
**     BTInorder ....... nerekurzivní průchod typu in-order
**     BTPostorder ..... nerekurzivní průchod typu post-order
**     BTDisposeTree ... zruš všechny uzly stromu
**
** xschul06 <3
** U všech funkcí, které využívají některý z průchodů stromem, implementujte
** pomocnou funkci pro nalezení nejlevějšího uzlu v podstromu.
**
** Přesné definice typů naleznete v souboru c402.h. Uzel stromu je typu tBTNode,
** ukazatel na něj je typu tBTNodePtr. Jeden uzel obsahuje položku int Cont,
** která současně slouží jako užitečný obsah i jako vyhledávací klíč
** a ukazatele na levý a pravý podstrom (LPtr a RPtr).
**
** Příklad slouží zejména k procvičení nerekurzivních zápisů algoritmů
** nad stromy. Než začnete tento příklad řešit, prostudujte si důkladně
** principy převodu rekurzivních algoritmů na nerekurzivní. Programování
** je především inženýrská disciplína, kde opětné objevování Ameriky nemá
** místo. Pokud se Vám zdá, že by něco šlo zapsat optimálněji, promyslete
** si všechny detaily Vašeho řešení. Povšimněte si typického umístění akcí
** pro různé typy průchodů. Zamyslete se nad modifikací řešených algoritmů
** například pro výpočet počtu uzlů stromu, počtu listů stromu, výšky stromu
** nebo pro vytvoření zrcadlového obrazu stromu (pouze popřehazování ukazatelů
** bez vytváření nových uzlů a rušení starých).
**
** Při průchodech stromem použijte ke zpracování uzlu funkci BTWorkOut().
** Pro zjednodušení práce máte předem připraveny zásobníky pro hodnoty typu
** bool a tBTNodePtr. Pomocnou funkci BTWorkOut ani funkce pro práci
** s pomocnými zásobníky neupravujte
** Pozor! Je třeba správně rozlišovat, kdy použít dereferenční operátor *
** (typicky při modifikaci) a kdy budeme pracovat pouze se samotným ukazatelem
** (např. při vyhledávání). V tomto příkladu vám napoví prototypy funkcí.
** Pokud pracujeme s ukazatelem na ukazatel, použijeme dereferenci.
**/

#include "c402.h"
int solved;

void BTWorkOut (tBTNodePtr Ptr)		{
/*   ---------
** Pomocná funkce, kterou budete volat při průchodech stromem pro zpracování
** uzlu určeného ukazatelem Ptr. Tuto funkci neupravujte.
**/

	if (Ptr==NULL)
    printf("Chyba: Funkce BTWorkOut byla volána s NULL argumentem!\n");
  else
    printf("Výpis hodnoty daného uzlu> %d\n",Ptr->Cont);
}

/* -------------------------------------------------------------------------- */
/*
** Funkce pro zásobník hotnot typu tBTNodePtr. Tyto funkce neupravujte.
**/

void SInitP (tStackP *S)
/*   ------
** Inicializace zásobníku.
**/
{
	S->top = 0;
}

void SPushP (tStackP *S, tBTNodePtr ptr)
/*   ------
** Vloží hodnotu na vrchol zásobníku.
**/
{
                 /* Při implementaci v poli může dojít k přetečení zásobníku. */
  if (S->top==MAXSTACK)
    printf("Chyba: Došlo k přetečení zásobníku s ukazateli!\n");
  else {
		S->top++;
		S->a[S->top]=ptr;
	}
}

tBTNodePtr STopPopP (tStackP *S)
/*         --------
** Odstraní prvek z vrcholu zásobníku a současně vrátí jeho hodnotu.
**/
{
                            /* Operace nad prázdným zásobníkem způsobí chybu. */
	if (S->top==0)  {
		printf("Chyba: Došlo k podtečení zásobníku s ukazateli!\n");
		return(NULL);
	}
	else {
		return (S->a[S->top--]);
	}
}

bool SEmptyP (tStackP *S)
/*   -------
** Je-li zásobník prázdný, vrátí hodnotu true.
**/
{
  return(S->top==0);
}

/* -------------------------------------------------------------------------- */
/*
** Funkce pro zásobník hotnot typu bool. Tyto funkce neupravujte.
*/

void SInitB (tStackB *S) {
/*   ------
** Inicializace zásobníku.
**/

	S->top = 0;
}

void SPushB (tStackB *S,bool val) {
/*   ------
** Vloží hodnotu na vrchol zásobníku.
**/
                 /* Při implementaci v poli může dojít k přetečení zásobníku. */
	if (S->top==MAXSTACK)
		printf("Chyba: Došlo k přetečení zásobníku pro boolean!\n");
	else {
		S->top++;
		S->a[S->top]=val;
	}
}

bool STopPopB (tStackB *S) {
/*   --------
** Odstraní prvek z vrcholu zásobníku a současně vrátí jeho hodnotu.
**/
                            /* Operace nad prázdným zásobníkem způsobí chybu. */
	if (S->top==0) {
		printf("Chyba: Došlo k podtečení zásobníku pro boolean!\n");
		return(NULL);
	}
	else {
		return(S->a[S->top--]);
	}
}

bool SEmptyB (tStackB *S) {
/*   -------
** Je-li zásobník prázdný, vrátí hodnotu true.
**/
  return(S->top==0);
}

/* -------------------------------------------------------------------------- */
/*
** Následuje jádro domácí úlohy - funkce, které máte implementovat.
*/

void BTInit (tBTNodePtr *RootPtr)	{
/*   ------
** Provede inicializaci binárního vyhledávacího stromu.
**
** Inicializaci smí programátor volat pouze před prvním použitím binárního
** stromu, protože neuvolňuje uzly neprázdného stromu (a ani to dělat nemůže,
** protože před inicializací jsou hodnoty nedefinované, tedy libovolné).
** Ke zrušení binárního stromu slouží procedura BTDisposeTree.
**
** Všimněte si, že zde se poprvé v hlavičce objevuje typ ukazatel na ukazatel,
** proto je třeba při práci s RootPtr použít dereferenční operátor *.
**/
    if ((*RootPtr) == NULL)
        return;

    (*RootPtr)->LPtr = NULL;
    (*RootPtr)->RPtr = NULL;
    (*RootPtr)->Cont = -1;
}

tBTNodePtr createNode(int cont, tBTNodePtr lptr, tBTNodePtr rptr) {
    tBTNodePtr newNode = malloc(sizeof *newNode);
    if (newNode == NULL)
        return NULL;

    newNode->Cont = cont;
    newNode->LPtr = lptr;
    newNode->RPtr = rptr;

    return newNode;
}

void BTInsert (tBTNodePtr *RootPtr, int Content) {
/*   --------
** Vloží do stromu nový uzel s hodnotou Content.
**
** Z pohledu vkládání chápejte vytvářený strom jako binární vyhledávací strom,
** kde uzly s hodnotou menší než má otec leží v levém podstromu a uzly větší
** leží vpravo. Pokud vkládaný uzel již existuje, neprovádí se nic (daná hodnota
** se ve stromu může vyskytnout nejvýše jednou). Pokud se vytváří nový uzel,
** vzniká vždy jako list stromu. Funkci implementujte nerekurzivně.
**/
    // zadny element
    if ((*RootPtr) == NULL) {
        tBTNodePtr newNode = createNode(Content, NULL, NULL);
        if (newNode)
            (*RootPtr) = newNode;
        return;
    }

    tBTNodePtr node = (*RootPtr);
    while (node != NULL) {
        // stopaz, nasli sme element co tam uz je
        if (node->Cont == Content)
            return;

        // jdeme doprava - vetsi nez otec
        if (Content > node->Cont) {
            if (node->RPtr != NULL) {
                node = node->RPtr;
                continue;
            }
                // nemuzeme doprava, sme na listu
            else {
                tBTNodePtr newNode = createNode(Content, NULL, NULL);
                if (newNode)
                    node->RPtr = newNode;

                return;
            }}

        // jdeme doleva - mensi nez otec
        if (Content < node->Cont) {
            // muzeme doleva
            if (node->LPtr != NULL) {
                node = node->LPtr;
                continue;
            }
            // nemuzeme doleva, sme na listu
            else {
                tBTNodePtr newNode = createNode(Content, NULL, NULL);
                if (newNode)
                    node->LPtr = newNode;

                return;
            }}

    }
}

/*                                  PREORDER                                  */

void Leftmost_Preorder (tBTNodePtr ptr, tStackP *Stack)	{
/*   -----------------
** Jde po levě větvi podstromu, dokud nenarazí na jeho nejlevější uzel.
**
** Při průchodu Preorder navštívené uzly zpracujeme voláním funkce BTWorkOut()
** a ukazatele na ně is uložíme do zásobníku.
**/
    if (ptr == NULL || (Stack) == NULL)
        return;

    tBTNodePtr node = ptr;
    while(node != NULL) {
        // ulozit
        SPushP(Stack, node);
        // zpracovat
        BTWorkOut(node);
        node = node->LPtr;
    }

    return;
}

void BTPreorder (tBTNodePtr RootPtr)	{
/*   ----------
** Průchod stromem typu preorder implementovaný nerekurzivně s využitím funkce
** Leftmost_Preorder a zásobníku ukazatelů. Zpracování jednoho uzlu stromu
** realizujte jako volání funkce BTWorkOut().
**/
    // podle prednasky
    if (RootPtr == NULL)
        return;

    tStackP *stack = malloc(sizeof(tStackP));
    if (stack == NULL)
        return;

    SInitP(stack);
    Leftmost_Preorder(RootPtr, stack);
    while (!SEmptyP(stack)) {
        tBTNodePtr ptr = STopPopP(stack);
        if (ptr->RPtr != NULL)
            Leftmost_Preorder(ptr->RPtr, stack);
    }

    free(stack);
}


void Leftmost_Inorder(tBTNodePtr ptr, tStackP *Stack)		{

    if (ptr == NULL || (Stack) == NULL)
        return;

    tBTNodePtr node = ptr;
    while(node != NULL) {
        // zpracovat
        BTWorkOut(node);
        // ulozit
        SPushP(Stack, node);
        node = node->LPtr;
    }

    return;

}

void BTInorder (tBTNodePtr RootPtr)	{
    if (RootPtr == NULL)
        return;

    tStackP *stack = malloc(sizeof(tStackP));
    if (stack == NULL )
        return;

    SInitP(stack);
    tBTNodePtr node = RootPtr;
    while (!SEmptyP(stack)) {
        node = STopPopP(stack);
        BTWorkOut(node);
        Leftmost_Inorder(node->RPtr, stack);
    }
    free(stack);
}

/*                                 POSTORDER                                  */

void Leftmost_Postorder (tBTNodePtr ptr, tStackP *StackP, tStackB *StackB) {
/*           --------
** Jde po levě větvi podstromu, dokud nenarazí na jeho nejlevější uzel.
**
** Při průchodu Postorder ukládáme ukazatele na navštívené uzly do zásobníku
** a současně do zásobníku bool hodnot ukládáme informaci, zda byl uzel
** navštíven poprvé a že se tedy ještě nemá zpracovávat.
**/
    while(ptr!= NULL){
        SPushP(StackP, ptr);
        SPushB(StackB, true);
        ptr= ptr->LPtr;
    }
}

void BTPostorder (tBTNodePtr RootPtr) {
/*           -----------
** Průchod stromem typu postorder implementovaný nerekurzivně s využitím funkce
** Leftmost_Postorder, zásobníku ukazatelů a zásobníku hotdnot typu bool.
** Zpracování jednoho uzlu stromu realizujte jako volání funkce BTWorkOut().
**/
    if (RootPtr == NULL)
        return;
    bool zleva;
    tStackP *stack = malloc(sizeof(tStackP));
    tStackB *stackb = malloc(sizeof(tStackB));
    if (stack == NULL || stackb == NULL)
        return;

    SInitP(stack);
    SInitB(stackb);
    tBTNodePtr node = RootPtr;

    Leftmost_Postorder(node, stack, stackb);
    while (!SEmptyP(stack)) {
        node = STopPopP(stack);
        zleva = STopPopB(stackb);
        if (zleva) {
            SPushP(stack, node);
            SPushB(stackb, false);
            Leftmost_Postorder(node->RPtr, stack, stackb);
        }else {
            BTWorkOut(node);
        }
    }
    free(stack);
    free(stackb);
}


void NejlevZrus(tBTNodePtr node, tStackP* stack){
    if (node == NULL)
        return;
    while (node != NULL) {
        SPushP(stack, node);
        node = node->LPtr;
    }
}

void BTDisposeTree (tBTNodePtr *RootPtr)	{
/*   -------------
** Zruší všechny uzly stromu a korektně uvolní jimi zabranou paměť.
**
** Funkci implementujte nerekurzivně s využitím zásobníku ukazatelů.
**/
    // z prednasky
    if ((*RootPtr) == NULL)
        return;

    tBTNodePtr node = (*RootPtr);
    tStackP *stack = malloc(sizeof(tStackP));
    if (stack == NULL)
        return;

    SInitP(stack);

    NejlevZrus(node, stack);
    while (!SEmptyP(stack)) {
        node = STopPopP(stack);
        if (node->RPtr != NULL)
            NejlevZrus(node->RPtr, stack);
        free(node);
    }
    free(stack);
    (*RootPtr) = NULL;
}

/* konec c402.c */

