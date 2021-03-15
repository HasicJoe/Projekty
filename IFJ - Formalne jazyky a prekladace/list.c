/**
 * @file list.c
 * @author Jonáš Tichý (xtichy29@stud.fit.vutbr.cz)
 * @author Miroslav Štěpánek (xstepa68@stud.fit.vutbr.cz)
 * @brief Linked list implementation for parser recursive syntax analysis
 * @date 2020-10-22
 * 
 */
#include "list.h"

/**
 * @brief List initialization
 * 
 * @param L 
 */
void DLInitList(tList *L) {
    L->Act = NULL;
    L->First = NULL;
    L->Last = NULL;
}

/**
 * @brief Free tokens and list pointers
 * 
 * @param L 
 */
void DLDisposeTokenList(tList *L) {
    if (L->First != NULL) {
        while (L->First != NULL) {
            tDLElemPtr tmp = L->First;
            if (tmp->data != NULL) {
                Token *data = (Token*)tmp->data;
                if (data->type == TOKEN_TYPE_IDENTIFIER || data->type == TOKEN_TYPE_STRING) {
                    free(data->attribute.string);
                }
                free(data);
            }
            L->First = tmp->rptr;
            free(tmp);
        }
    }
    L->Act = NULL;
    L->First = NULL;
    L->Last = NULL;
}

/**
 * @brief Insert element after active into the list
 * 
 * @param L 
 * @param tkn 
 * @return int 
 */
int DLInsertAct(tList *L, void *tkn) {
    if (L->Act != NULL) {
        tDLElemPtr item = (tDLElemPtr) malloc(sizeof(struct tDLElem));
        if (item == NULL) {
            return INTERNAL_ERR;
        }	
        item->data = tkn;
        item->rptr = L->Act->rptr;
        L->Act->rptr = item;
        if (L->Last == L->Act) {
            L->Last = item;
        }
    }
    return 0;
}

/**
 * @brief Insert element as last into the list
 * 
 * @param L 
 * @param tkn 
 * @return int 
 */
int DLInsertLast(tList *L, void *tkn) {
    tDLElemPtr item = (tDLElemPtr) malloc(sizeof(struct tDLElem));
    if (item == NULL) {
        return INTERNAL_ERR;
    }	
    item->data = tkn;
    item->rptr = NULL;
    if (L->Last != NULL) {
        L->Last->rptr = item;
        L->Last = item;
    } else {
        L->First = item;
        L->Last = item;
    }
    return 0;
}

/**
 * @brief Insert element as first into the list
 * 
 * @param L 
 * @param tkn 
 * @return int 
 */
int DLInsertFirst(tList *L, void *tkn) {
    tDLElemPtr item = (tDLElemPtr) malloc(sizeof(struct tDLElem));
    if (item == NULL) {
        return INTERNAL_ERR;
    }
    item->data = tkn;
    item->rptr = NULL;
    if (L->First != NULL) {
        item->rptr = L->First;
    } else {
        L->Last = item;
    }
    L->First = item;
    return 0;
}

/**
 * @brief Delete the last pointer to element in the list, element needs to be freed before
 * 
 * @param L 
 */
void DLDeleteLast(tList *L) {
    if (L->Last != NULL) {
        tDLElemPtr tmp = L->Last;
        if (L->Last == L->First) {
            L->First = NULL;
            L->Last = NULL;
        } else {
            tDLElemPtr ptr = L->First;
            while (ptr->rptr != L->Last) {
                ptr = ptr->rptr;
            }
            L->Last = ptr;
        }
        free(tmp);
    }
}

/**
 * @brief Delete the first pointer to element in the list, element needs to be freed before
 * 
 * @param L 
 */
void DLDeleteFirst(tList *L) {
    if (L->First != NULL) {
        tDLElemPtr tmp = L->First;
        if (L->First->rptr == NULL) {
            L->First = NULL;
        } else {
            L->First = tmp->rptr;
        }
        free(tmp);
    }
}

/**
 * @brief Free the list of element pointers
 * 
 * @param L 
 */
void DLDisposeSafe(tList *L) {
    if (L->First != NULL) {
        while (L->First != NULL) {
            tDLElemPtr tmp = L->First;
            L->First = tmp->rptr;
            free(tmp);
        }
    }
    L->Act = NULL;
    L->First = NULL;
    L->Last = NULL;
}

/**
 * @brief Count number of elements in list
 * @author Samuel Valaštín
 * @param L 
 * @return int 
 */
int countList(tList *L){
    int count = 0;
    if(L->First != NULL){
        tDLElemPtr tmp = L->First;
        while(tmp != NULL){
            count++;
            tmp = tmp->rptr;
        }
        return count;
    }
    return 0;
}