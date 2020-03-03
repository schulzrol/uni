//======== Copyright (c) 2017, FIT VUT Brno, All rights reserved. ============//
//
// Purpose:     Test Driven Development - priority queue code
//
// $NoKeywords: $ivs_project_1 $tdd_code.cpp
// $Author:     JMENO PRIJMENI <xlogin00@stud.fit.vutbr.cz>
// $Date:       $2017-01-04
//============================================================================//
/**
 * @file tdd_code.cpp
 * @author Roland Schulz
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
    this->head = NULL;
}

PriorityQueue::~PriorityQueue()
{
    while (this->head != NULL) {
        this->RemoveFirst();
    }
}

void PriorityQueue::Insert(int value)
{
    // dont know how to handle out of mem throws due to not documented behaviour
    // in assessment, so i assume theres always enough memory
    PriorityQueue::Element_t *newEm = new PriorityQueue::Element_t;
    
    newEm->value = value;
    newEm->pPrev = NULL;
    newEm->pNext = NULL;

    PriorityQueue::Element_t *current = this->head;
    PriorityQueue::Element_t *lastVisit = NULL;
    // find the element after which to insert (or after last element), then break
    while (current != NULL) {
        if (current->value >= value) {
            break;
        }
        lastVisit = current;
        current = current->pNext;
    }

    // if list empty or new head insert, initiate head
    if (lastVisit == NULL) {
        // new head insert
        if (this->head != NULL) {
            this->head->pPrev = newEm;
            newEm->pNext = this->head;
        }
        this->head = newEm;
        return;
    }

    this->InsertElementAfter(lastVisit, newEm);
    return;
}

bool PriorityQueue::Remove(int value)
{
    PriorityQueue::Element_t *found = this->Find(value);
    if (found != NULL) {
        this->RemoveElement(found);
    }

    return (found == NULL) ? false : true;
}

PriorityQueue::Element_t *PriorityQueue::Find(int value)
{
    PriorityQueue::Element_t *current = this->head;
    while (current != NULL) {
        if (current->value == value) {
            return current;
        }
        current = current->pNext;
    }

    return NULL;
}

PriorityQueue::Element_t *PriorityQueue::GetHead()
{
    return this->head;
}

void PriorityQueue::RemoveElement(PriorityQueue::Element_t *em)
{
    if (em == NULL) {return;}

    // case that em is not last
    if (em->pNext != NULL) {
        em->pNext->pPrev = em->pPrev;
    }
    
    // case that em is not first
    if (em->pPrev != NULL) {
        em->pPrev->pNext = em->pNext;
    }

    // case that em is the only one
    if (em->pPrev == NULL) {
        this->RemoveFirst();
        return;
    }

    delete em;
    return; 
}

void PriorityQueue::RemoveFirst()
{
    if (this->head == NULL) {return;}

    // save head for deleting later
    PriorityQueue::Element_t* tmp = this->head;
    // set head as the next em
    this->head = tmp->pNext;
    if (this->head != NULL) {
        this->head->pPrev = NULL;
    }

    delete tmp;

    return;
}

void PriorityQueue::InsertElementAfter(PriorityQueue::Element_t* after, PriorityQueue::Element_t *em)
{
    if (em == NULL) {return;}

    em->pPrev = after;
    
    // means its first(head)
    if (after == NULL) {
        em->pNext = NULL;
        return;
    }

    em->pNext = after->pNext;
    after->pNext = em;
    if (em->pNext != NULL) {
        em->pNext->pPrev = em;
    }

    return;
}

/*** Konec souboru tdd_code.cpp ***/
