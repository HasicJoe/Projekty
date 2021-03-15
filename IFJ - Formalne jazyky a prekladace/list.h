/**
 * @file list.h
 * @author Jonáš Tichý (xtichy29@stud.fit.vutbr.cz)
 * @author Miroslav Štěpánek (xstepa68@stud.fit.vutbr.cz)
 * @brief Linked list implementation for parser recursive syntax analysis
 * @date 2020-10-22
 * 
 */
#ifndef LIST
#define LIST

#include<stdio.h>
#include<stdlib.h>
#include "scanner.h"
#include "error.h"

typedef struct tDLElem {
        void *data;
        struct tDLElem *rptr;
} *tDLElemPtr;

typedef struct {
    tDLElemPtr First;
    tDLElemPtr Act;
    tDLElemPtr Last;
} tList;

int countList(tList *);
int DLInsertAct(tList *, void *);
int DLInsertLast(tList *, void *);
int DLInsertFirst(tList *, void *);
void DLInitList (tList *);
void DLDeleteLast(tList *);
void DLDeleteFirst(tList *);
void DLDisposeSafe (tList *);
void DLDisposeTokenList (tList *);

#endif