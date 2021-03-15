
/* c201.c *********************************************************************}
{* Téma: Jednosměrný lineární seznam
**
**                     Návrh a referenční implementace: Petr Přikryl, říjen 1994
**                                          Úpravy: Andrea Němcová listopad 1996
**                                                   Petr Přikryl, listopad 1997
**                                Přepracované zadání: Petr Přikryl, březen 1998
**                                  Přepis do jazyka C: Martin Tuček, říjen 2004
**                                              Úpravy: Kamil Jeřábek, září 2020
**
** Implementujte abstraktní datový typ jednosměrný lineární seznam.
** Užitečným obsahem prvku seznamu je celé číslo typu int.
** Seznam bude jako datová abstrakce reprezentován proměnnou typu tList.
** Definici konstant a typů naleznete v hlavičkovém souboru c201.h.
**
** Vaším úkolem je implementovat následující operace, které spolu s výše
** uvedenou datovou částí abstrakce tvoří abstraktní datový typ tList:
**
**      DisposeList ... zrušení všech prvků seznamu,
**      InitList ...... inicializace seznamu před prvním použitím,
**      InsertFirst ... vložení prvku na začátek seznamu,
**      First ......... nastavení aktivity na první prvek,
**      CopyFirst ..... vrací hodnotu prvního prvku,
**      DeleteFirst ... zruší první prvek seznamu,
**      PostDelete .... ruší prvek za aktivním prvkem,
**      PostInsert .... vloží nový prvek za aktivní prvek seznamu,
**      Copy .......... vrací hodnotu aktivního prvku,
**      Actualize ..... přepíše obsah aktivního prvku novou hodnotou,
**      Succ .......... posune aktivitu na další prvek seznamu,
**      Active ........ zjišťuje aktivitu seznamu.
**
** Při implementaci funkcí nevolejte žádnou z funkcí implementovaných v rámci
** tohoto příkladu, není-li u dané funkce explicitně uvedeno něco jiného.
**
** Nemusíte ošetřovat situaci, kdy místo legálního ukazatele na seznam předá
** někdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodně komentujte!
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako
** procedury (v jazyce C procedurám odpovídají funkce vracející typ void).
**/

#include "c201.h"

int errflg;
int solved;

void Error() {
/*
** Vytiskne upozornění na to, že došlo k chybě.
** Tato funkce bude volána z některých dále implementovaných operací.
**/
    printf ("*ERROR* The program has performed an illegal operation.\n");
    errflg = TRUE;                      /* globální proměnná -- příznak chyby */
}

void InitList (tList *L) {
/*
** Provede inicializaci seznamu L před jeho prvním použitím (tzn. žádná
** z následujících funkcí nebude volána nad neinicializovaným seznamem).
** Tato inicializace se nikdy nebude provádět nad již inicializovaným
** seznamem, a proto tuto možnost neošetřujte. Vždy předpokládejte,
** že neinicializované proměnné mají nedefinovanou hodnotu.
**/
    L->Act = NULL;
    L->First = NULL;
}

void DisposeList (tList *L) {
/*
** Zruší všechny prvky seznamu L a uvede seznam L do stavu, v jakém se nacházel
** po inicializaci. Veškerá paměť používaná prvky seznamu L bude korektně
** uvolněna voláním operace free.
***/
	
    while(L->First != NULL)
    {
        /* použijeme pomocný pointer aby sme nestratili previazanosť zoznamu */
        tElemPtr ptr = L->First; 
        free(ptr);
        L->First = L->First->ptr; // posunieme sa na dalsi prvok, po skonceni cyklu L->First = NULL
    }
    L->Act = NULL;  // uvedenie do stavu ako po inicializacii
}

void InsertFirst (tList *L, int val) {
/*
** Vloží prvek s hodnotou val na začátek seznamu L.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci Error().
**/

    /* Alokujeme pamäť pre nový prvok */
    tElemPtr newItem = (tElemPtr)malloc(sizeof(struct tElem)); 

    if(newItem == NULL)
    {
        Error();
        return;
    }
    else if(L->First == NULL)
    {
        /* Prvé vkladanie newItem-> ptr ostáva NULL */
        newItem->data = val;
        L->First = newItem;
    }
    else
    {
        /* Ďalšie vkladanie na začiatok zoznamu */
        newItem->data = val;
        newItem->ptr = L->First;
        L->First = newItem;
    }
}   

void First (tList *L) {
/*
** Nastaví aktivitu seznamu L na jeho první prvek.
** Funkci implementujte jako jediný příkaz, aniž byste testovali,
** zda je seznam L prázdný.
**/
    L->Act = L->First;
}

void CopyFirst (tList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu prvního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci Error().
**/
    if(L->First != NULL)
    {
        *val = L->First->data; 
        return;
    }
    Error();
}

void DeleteFirst (tList *L) {
/*
** Zruší první prvek seznamu L a uvolní jím používanou paměť.
** Pokud byl rušený prvek aktivní, aktivita seznamu se ztrácí.
** Pokud byl seznam L prázdný, nic se neděje.
**/

    if(L->First != NULL)      
    {
        if(L->First == L->Act)
        {
            L->Act = NULL;
        }

        if(L->First->ptr == NULL)
        {
            /* Ak prvá položka nemá následníka nepotrebujeme pomocný ukazateľ (optimalizácia) */
            free(L->First);
            return;
        }
        tElemPtr tmpPtr = L->First; // uložime si ukazatel na prvú položku v pomocnom ukazateli
        L->First = L->First->ptr; // posun na ďalšiu polozku
        free(tmpPtr); // uvolnenie pamäte
    }
}	

void PostDelete (tList *L) {
/* 
** Zruší prvek seznamu L za aktivním prvkem a uvolní jím používanou paměť.
** Pokud není seznam L aktivní nebo pokud je aktivní poslední prvek seznamu L,
** nic se neděje.
**/
    if(L->Act == NULL)
    {
        return;
    }
    else if ( L->Act->ptr == NULL)
    {
        return;
    }
    tElemPtr tmpPtr = L->Act->ptr; // pomocny ukazatel na nasledujucu polozku zoznamu
    L->Act->ptr = tmpPtr->ptr;  // nasledujuca bude polozka za vymazanou polozkou
    free(tmpPtr);
}

void PostInsert (tList *L, int val) {
/*
** Vloží prvek s hodnotou val za aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje!
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** zavolá funkci Error().
**/
	if(L->Act == NULL)    // kontrola či je zoznam aktívny
    {
        return;
    }
    tElemPtr newItem = (tElemPtr)malloc(sizeof(struct tElem));
    if(newItem == NULL)
    {
        Error();
        return;
    }
    newItem->ptr = L->Act->ptr; // previazanie novej polozky s naslednikom aktivnej polozky
    newItem->data = val;        // ulozenie hodnoty
    L->Act->ptr = newItem;      
}

void Copy (tList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu aktivního prvku seznamu L.
** Pokud seznam není aktivní, zavolá funkci Error().
**/

    if(L->Act != NULL)
    {
        *val = L->Act->data; 
        return;
    }
    Error();
}

void Actualize (tList *L, int val) {
/*
** Přepíše data aktivního prvku seznamu L hodnotou val.
** Pokud seznam L není aktivní, nedělá nic!
**/

    if(L->Act != NULL)
    {
        L->Act->data = val;
    }
}

void Succ (tList *L) {
/*
** Posune aktivitu na následující prvek seznamu L.
** Všimněte si, že touto operací se může aktivní seznam stát neaktivním.
** Pokud není předaný seznam L aktivní, nedělá funkce nic.
**/
    if(L->Act != NULL)
    {
        if(L->Act->ptr != NULL)
        {
            /* Presúvame aktivitu na nasledujúci prvok zoznamu */
            L->Act = L->Act->ptr;
        }
        else
        {
            /* Posledný prvok zoznamu -> neni kam posunúť aktivitu -> NULL */
            L->Act = NULL;
        }
    }
}

int Active (tList *L) {
/*
** Je-li seznam L aktivní, vrací nenulovou hodnotu, jinak vrací 0.
** Tuto funkci je vhodné implementovat jedním příkazem return. 
**/	
    return (L->Act != NULL);
}
/* Konec c201.c */
