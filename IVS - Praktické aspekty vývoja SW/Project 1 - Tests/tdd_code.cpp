//======== Copyright (c) 2021, FIT VUT Brno, All rights reserved. ============//
//
// Purpose:     Test Driven Development - priority queue code
//
// $NoKeywords: $ivs_project_1 $tdd_code.cpp
// $Author:     Samuel Valaštín <xvalas10@stud.fit.vutbr.cz>
// $Date:       $2021-02-25
//============================================================================//
/**
 * @file tdd_code.cpp
 * @author Samuel Valaštín
 * 
 * @brief Implementace metod tridy prioritni fronty.
 */

#include <stdlib.h>
#include <stdio.h>
#include "tdd_code.h"

//============================================================================//
// ** ZDE DOPLNTE IMPLEMENTACI **
//
// Zde doplnte implementaci verejneho rozhrani prioritni fronty (Priority Queue)
// 1. Verejne rozhrani fronty specifikovane v: tdd_code.h (sekce "public:")
//    - Konstruktor (PriorityQueue()), Destruktor (~PriorityQueue())
//    - Metody Insert/Remove/Find a GetHead
//    - Pripadne vase metody definovane v tdd_code.h (sekce "protected:")
//
// Cilem je dosahnout plne funkcni implementace prioritni fronty implementovane
// pomoci tzv. "double-linked list", ktera bude splnovat dodane testy 
// (tdd_tests.cpp).
//============================================================================//

PriorityQueue::PriorityQueue()
{
    m_pHead = new Element_t;   
    m_pHead->pNext = nullptr;
}

PriorityQueue::~PriorityQueue()
{
    Element_t *head = m_pHead;
    if(m_pHead->pNext){
        Element_t *tmpEl = m_pHead->pNext;
        Element_t *deletedEl;
        while(tmpEl->pNext != nullptr){
            deletedEl = tmpEl;
            tmpEl = tmpEl->pNext;
            delete deletedEl;
        }
        delete tmpEl;   // delete last element
    }
    delete m_pHead;
}

void PriorityQueue::Insert(int value)
{
    Element_t *insertElement = new Element_t;
    insertElement->value = value;
    insertElement->pNext = nullptr; // valgrind errors from context 

    // vkladame do prazdnej fronty
    if(m_pHead->pNext == nullptr){
        m_pHead->pNext = insertElement;
        insertElement->pNext = nullptr;
        return;
    }
    //vkladame na zaciatok fronty
    if(m_pHead->pNext->value < value){
        insertElement->pNext = m_pHead->pNext;
        m_pHead->pNext = insertElement;
        return;
    }
    // vkladame dalej
    Element_t *tmpEl = m_pHead->pNext;
    if(tmpEl->pNext != nullptr){
        while(tmpEl->pNext->value > value){
            if(tmpEl->pNext->pNext == nullptr){
                // vkladame na koniec
                tmpEl->pNext->pNext = insertElement;
                return;
            }
            // posuvame sa dalej vo fronte
            tmpEl = tmpEl->pNext;
        }
        // nasli sme miesto spravne previazeme frontu
        insertElement->pNext = tmpEl->pNext;
        tmpEl->pNext = insertElement;
    }
}

bool PriorityQueue::Remove(int value)
{
    Element_t *tmpEl = m_pHead->pNext;
    if(m_pHead->pNext == nullptr){
        return false;
    }
    // mazeme nazaciatku
    if(m_pHead->pNext->value == value){
        m_pHead->pNext = m_pHead->pNext->pNext;
        delete tmpEl;
        return true;
    }
    Element_t *prevtmpEl;
    while(tmpEl->value != value){
        if(tmpEl->pNext != nullptr){
            prevtmpEl = tmpEl;
            tmpEl = tmpEl->pNext;
        } else {
            return false;
        }
    }
    if(tmpEl->value == value){
       // if(tmpEl->pNext != NULL){                   kedze ide o frontu a nie o zoznam tak zakomentovavam
        //    prevtmpEl->pNext = tmpEl->pNext;
        //    delete tmpEl;
        //    return true;
       // } else {
            prevtmpEl->pNext = nullptr;
            delete tmpEl;
       // }
    }
    return true;
}

PriorityQueue::Element_t *PriorityQueue::Find(int value)
{
    if(m_pHead->pNext == nullptr){
        return NULL;
    }
    Element_t *tmpEl = m_pHead->pNext;

    while(tmpEl->value != value){
        if(tmpEl->pNext != nullptr){
            tmpEl = tmpEl->pNext;
        } else {
            return NULL;
        }
    }
    return tmpEl;  
}

size_t PriorityQueue::Length()
{
    if(m_pHead->pNext == nullptr){
        return 0;
    }
    size_t count = 1;
    Element_t *tmpEl = m_pHead->pNext;
    if(tmpEl->pNext){
       while(tmpEl->pNext){
           count++;
           tmpEl = tmpEl->pNext;
       }
    }
    return count;
}

PriorityQueue::Element_t *PriorityQueue::GetHead()
{
    if(m_pHead->pNext != nullptr){
        return m_pHead->pNext;
    }
    return nullptr;
}
/*** Konec souboru tdd_code.cpp ***/
