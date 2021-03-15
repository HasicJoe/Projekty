
/* c206.c **********************************************************}
{* Téma: Dvousměrně vázaný lineární seznam
**
**                   Návrh a referenční implementace: Bohuslav Křena, říjen 2001
**                            Přepracované do jazyka C: Martin Tuček, říjen 2004
**                                            Úpravy: Kamil Jeřábek, září 2020
**
** Implementujte abstraktní datový typ dvousměrně vázaný lineární seznam.
** Užitečným obsahem prvku seznamu je hodnota typu int.
** Seznam bude jako datová abstrakce reprezentován proměnnou
** typu tDLList (DL znamená Double-Linked a slouží pro odlišení
** jmen konstant, typů a funkcí od jmen u jednosměrně vázaného lineárního
** seznamu). Definici konstant a typů naleznete v hlavičkovém souboru c206.h.
**
** Vaším úkolem je implementovat následující operace, které spolu
** s výše uvedenou datovou částí abstrakce tvoří abstraktní datový typ
** obousměrně vázaný lineární seznam:
**
**      DLInitList ...... inicializace seznamu před prvním použitím,
**      DLDisposeList ... zrušení všech prvků seznamu,
**      DLInsertFirst ... vložení prvku na začátek seznamu,
**      DLInsertLast .... vložení prvku na konec seznamu,
**      DLFirst ......... nastavení aktivity na první prvek,
**      DLLast .......... nastavení aktivity na poslední prvek,
**      DLCopyFirst ..... vrací hodnotu prvního prvku,
**      DLCopyLast ...... vrací hodnotu posledního prvku,
**      DLDeleteFirst ... zruší první prvek seznamu,
**      DLDeleteLast .... zruší poslední prvek seznamu,
**      DLPostDelete .... ruší prvek za aktivním prvkem,
**      DLPreDelete ..... ruší prvek před aktivním prvkem,
**      DLPostInsert .... vloží nový prvek za aktivní prvek seznamu,
**      DLPreInsert ..... vloží nový prvek před aktivní prvek seznamu,
**      DLCopy .......... vrací hodnotu aktivního prvku,
**      DLActualize ..... přepíše obsah aktivního prvku novou hodnotou,
**      DLPred .......... posune aktivitu na předchozí prvek seznamu,
**      DLSucc .......... posune aktivitu na další prvek seznamu,
**      DLActive ........ zjišťuje aktivitu seznamu.
**
** Při implementaci jednotlivých funkcí nevolejte žádnou z funkcí
** implementovaných v rámci tohoto příkladu, není-li u funkce
** explicitně uvedeno něco jiného.
**
** Nemusíte ošetřovat situaci, kdy místo legálního ukazatele na seznam 
** předá někdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodně komentujte!
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako
** procedury (v jazyce C procedurám odpovídají funkce vracející typ void).
**/

#include "c206.h"

int solved;
int errflg;

void DLError() {
/*
** Vytiskne upozornění na to, že došlo k chybě.
** Tato funkce bude volána z některých dále implementovaných operací.
**/	
    printf ("*ERROR* The program has performed an illegal operation.\n");
    errflg = TRUE;             /* globální proměnná -- příznak ošetření chyby */
    return;
}

void DLInitList (tDLList *L) {
/*
** Provede inicializaci seznamu L před jeho prvním použitím (tzn. žádná
** z následujících funkcí nebude volána nad neinicializovaným seznamem).
** Tato inicializace se nikdy nebude provádět nad již inicializovaným
** seznamem, a proto tuto možnost neošetřujte. Vždy předpokládejte,
** že neinicializované proměnné mají nedefinovanou hodnotu.
**/
    L->Act=NULL;
    L->First=NULL;
    L->Last=NULL;
}

void DLDisposeList (tDLList *L) {
/*
** Zruší všechny prvky seznamu L a uvede seznam do stavu, v jakém
** se nacházel po inicializaci. Rušené prvky seznamu budou korektně
** uvolněny voláním operace free. 
**/
	
    if(L->Act != NULL)
    {
        if(L->First != NULL)
        {
            while(L->First != NULL)
            {
                tDLElemPtr tmpPtr = L->First; // pomocny ukazatel na prichytenie prveho prvku
                L->First = L->First->rptr; // posun
                free(tmpPtr);
            }
            L->Act = NULL;
            L->Last = NULL;
            return;         
        }
    }
}

void DLInsertFirst (tDLList *L, int val) {
/*
** Vloží nový prvek na začátek seznamu L.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
	tDLElemPtr newFirstItem = (tDLElemPtr)malloc(sizeof(struct tDLElem));
    if(newFirstItem == NULL)
    {
        DLError();
        return;
    }

    if(L->First == NULL)
    {
        if(L->Last == NULL)
        {
            // vlozenie  prveho prvku do prázdneho zoznamu
            newFirstItem->data = val; 
            L->First = newFirstItem;
            L->Last = newFirstItem;
            return;
        }
    }
    newFirstItem->data = val; // nahranie hodnoty do nového prvého prvku zoznamu
    newFirstItem->rptr = L->First; // previazanie nového prvku s predchádzajucim prvým prvkom
    newFirstItem->lptr = NULL;
    L->First->lptr = newFirstItem; // previazanie predchadzajuceho prveho prvku
    L->First = newFirstItem; // zmena prveho prvku
}

void DLInsertLast(tDLList *L, int val) {
/*
** Vloží nový prvek na konec seznamu L (symetrická operace k DLInsertFirst).
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/ 
    tDLElemPtr newLastItem = (tDLElemPtr)malloc(sizeof(struct tDLElem));
    if(newLastItem == NULL)
    {
        DLError();
        return;
    }

    if(L->First == NULL)
    {
        if(L->Last == NULL)
        {
            // vlozenie prveho prvku do prázdneho zoznamu
            newLastItem->data = val;
            L->Last = newLastItem;
            L->First = newLastItem;
            return;
        }
    }
        newLastItem->data = val; // nahranie hodnoty do noveho prvku
        newLastItem->lptr = L->Last; // previazanie s predchadzajucim poslednym prvkom
        newLastItem->rptr = NULL;
        L->Last->rptr= newLastItem; // previazanie predchadzajuceho posledneho prvku s novym poslednym
        L->Last = newLastItem; // novym poslednym prvkom zoznamu sa stal novo alokovany prvok	
}

void DLFirst (tDLList *L) {
/*
** Nastaví aktivitu na první prvek seznamu L.
** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
** aniž byste testovali, zda je seznam L prázdný.
**/
    L->Act = L->First;
}

void DLLast (tDLList *L) {
/*
** Nastaví aktivitu na poslední prvek seznamu L.
** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
** aniž byste testovali, zda je seznam L prázdný.
**/
    L->Act = L->Last;
}

void DLCopyFirst (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu prvního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/
    if(L->First == NULL)
    {
        if(L->Last == NULL)
        {
            DLError();
            return;
        }
    }
    *val = L->First->data;
}

void DLCopyLast (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu posledního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/
    if(L->First == NULL)
    {
        if(L->Last == NULL)
        {
            DLError();
            return;
        }
    }
    *val = L->Last->data;
}

void DLDeleteFirst (tDLList *L) {
/*
** Zruší první prvek seznamu L. Pokud byl první prvek aktivní, aktivita 
** se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
**/ 
    if(L->First == NULL)
    {
        return;
    }

    if(L->First == L->Act)
    {
        L->Act = NULL;
        if(L->First->rptr == NULL)
        {
            free(L->First);
            // kedze rusime posledny prvok musime zrusit aj ukazatele na zaciatok a koniec
            L->Last = NULL;
            L->First = NULL;
        }
        else
        {
            tDLElemPtr tmpPtr = L->First; // pomocny pointer na naviazanie prveho prvku
            L->First = L->First->rptr; // novym prvym prvkom sa stava povodne druhy prvok
            L->First->lptr = NULL;
            free(tmpPtr);
        }
    }
    else
    {
        if(L->First->rptr == NULL)
        {
            free(L->First);
            L->Last = NULL;
            L->First = NULL;
        }
        else
        {
            tDLElemPtr tmpPtr = L->First; // pomocny pointer na naviazanie prveho prvku
            L->First = L->First->rptr; // novym prvym prvkom sa stava povodne druhy prvok
            L->First->lptr = NULL;
            free(tmpPtr);
        }   
    }
}	

void DLDeleteLast (tDLList *L) {
/*
** Zruší poslední prvek seznamu L.
** Pokud byl poslední prvek aktivní, aktivita seznamu se ztrácí.
** Pokud byl seznam L prázdný, nic se neděje.
**/
    if(L->Last == NULL)
    {
        return;
    }

    if(L->Last == L->Act) // pokial bol posledny prvok aktivny tak sa aktivita straca
    {
        L->Act = NULL;
        if(L->Last->lptr == NULL)
        {
            free(L->Last); // pokial je jediny prvok mazany
            L->Last = NULL;
            L->First = NULL;
        }
        else
        {
            tDLElemPtr tmpPtr = L->Last; // pomocny pointer na naviazanie prveho prvku
            L->Last = L->Last->lptr; // novym prvym prvkom sa stava povodne druhy prvok
            L->Last->rptr = NULL;
            free(tmpPtr);
        }
    }
    else
    {
        if(L->Last->lptr == NULL)
        {
            free(L->Last);
            L->First = NULL;
            L->Last = NULL;
        }
        else
        {
            tDLElemPtr tmpPtr = L->Last; // pomocny pointer na naviazanie prveho prvku
            L->Last = L->Last->lptr; // novym prvym prvkom sa stava povodne druhy prvok
            L->Last->rptr = NULL;
            tmpPtr->lptr = NULL;
            free(tmpPtr);
        }   
    }	 
}

void DLPostDelete (tDLList *L) {
/*
** Zruší prvek seznamu L za aktivním prvkem.
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** posledním prvkem seznamu, nic se neděje.
**/
    if(L->Act == L->Last)
    {
        return; // osetrenie aktivny prvok == posledny prvok zoznamu
    }

    if(L->Act != NULL)
    {
        if(L->Act->rptr != NULL)
        {
            // kontrola ci je zoznam aktivny a ci neni aktivna polozka poslednou polozkou
            tDLElemPtr tmpPtr = L->Act->rptr;   // prichytime si mazany prvok
            if(tmpPtr->rptr != NULL)
            {
                L->Act->rptr = tmpPtr->rptr; 
                tmpPtr->lptr = L->Act;          // previazeme zoznam pred mazanim
                free(tmpPtr);
            }
            else
            {
                L->Last = L->Act;               // poslednym prvkom zoznamu sa stava aktivny
                L->Last->rptr = NULL;
                free(tmpPtr);
            }
        }
    }
}

void DLPreDelete (tDLList *L) {
/*
** Zruší prvek před aktivním prvkem seznamu L .
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** prvním prvkem seznamu, nic se neděje.
**/
    if(L->Act == L->First)
    {
        return; // osetrime pripad ak je aktivny prvok prvym 
    }

    if(L->Act != NULL)
    {
        if(L->Act->lptr != NULL)
        {
            tDLElemPtr tmpPtr = L->Act->lptr;   // prichytime si mazany prvok
            if(tmpPtr->lptr != NULL)            // ak ma mazany prvok predchodcu
            {
                L->Act->lptr = tmpPtr->lptr;    
                tmpPtr->lptr->rptr = L->Act;    // previazeme zoznam aby sme mohli mazat
                free(tmpPtr);
            }
            else
            {
                L->First = L->Act;              // prvym prvkom sa stava aktualny
                L->First->lptr = NULL; // zrusime ukazatel na predchodcu
                free(tmpPtr);
            }
        }
    }
}

void DLPostInsert (tDLList *L, int val) {
/*
** Vloží prvek za aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
    if(L->Act != NULL)
    {
        tDLElemPtr newItem = (tDLElemPtr)malloc(sizeof(struct tDLElem));
        if(newItem == NULL)
        {
            DLError();
            return;
        }
        newItem->data = val; // priradenie hodnoty do noveho prvku

        if(L->Act->rptr != NULL)
        {
            // prepojenie noveho prvku do zoznamu ak ma aktivny prvok naslednika
            L->Act->rptr->lptr = newItem;
            newItem->lptr = L->Act;
            newItem->rptr = L->Act->rptr;
            L->Act->rptr = newItem;
        }
        else
        {
            // vlozenie prvku za aktivny novy prvok sa stava zaroven poslednym
            L->Act->rptr = newItem;
            newItem->lptr = L->Act;
            L->Last = newItem;
        }
    }
}

void DLPreInsert (tDLList *L, int val) {
/*
** Vloží prvek před aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/

    if(L->Act != NULL)
    {
        tDLElemPtr newItem = (tDLElemPtr)malloc(sizeof(struct tDLElem));
        if(newItem == NULL)
        {
            DLError();
            return;
        }
        newItem->data = val; // priradenie hodnoty do noveho prvku

        if(L->Act->lptr != NULL)
        {
            
            // prepojenie noveho prvku do zoznamu ak ma aktivny prvok predchodcu
            L->Act->lptr->rptr = newItem;
            newItem->rptr = L->Act;
            newItem->lptr = L->Act->lptr;
            L->Act->lptr = newItem;
        }
        else
        {
            // vlozenie prvku na prve miesto pred aktivny prvok
            L->Act->lptr = newItem;
            newItem->rptr = L->Act;
            L->First = newItem;
        }
    }
}

void DLCopy (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu aktivního prvku seznamu L.
** Pokud seznam L není aktivní, volá funkci DLError ().
**/
    if(L->Act != NULL)
    {
        *val = L->Act->data;
        return;
    }
    DLError();
}

void DLActualize (tDLList *L, int val) {
/*
** Přepíše obsah aktivního prvku seznamu L.
** Pokud seznam L není aktivní, nedělá nic.
**/
    if(L->Act != NULL)
    {
        L->Act->data = val;
    }
}

void DLSucc (tDLList *L) {
/*
** Posune aktivitu na následující prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na posledním prvku se seznam stane neaktivním.
**/
	if(L->Act != NULL)
    {
        if(L->Act->rptr == NULL)
        {
            L->Act = NULL; // pokial nema prvok nasledovnika stava sa neaktivnym
        }
        else if(L->First == L->Last)
        {
            L->Act = NULL; // pokial mame jeden prvok v zozname nemame sa kam dalej posunut
        }
        else
        {
            L->Act = L->Act->rptr;
        }
    }
}

void DLPred (tDLList *L) {
/*
** Posune aktivitu na předchozí prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na prvním prvku se seznam stane neaktivním.
**/
	
	if(L->Act != NULL)
    {
        if(L->Act->lptr == NULL)
        {
            L->Act = NULL; // pokial nema prvok predchodcu stava sa neaktivnym
        }
        else if(L->First == L->Last)
        {
            L->Act = NULL; // pokial mame jeden prvok v zozname nemame sa kam  posunut
        }
        else
        {
            L->Act = L->Act->lptr;
        }
    }
}

int DLActive (tDLList *L) {
/*
** Je-li seznam L aktivní, vrací nenulovou hodnotu, jinak vrací 0.
** Funkci je vhodné implementovat jedním příkazem return.
**/
    return (L->Act != NULL);
}
/* Konec c206.c*/
