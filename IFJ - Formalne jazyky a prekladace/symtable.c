/**
 * @file symtable.c
 * @author Jonáš Tichý (xtichy29@stud.fit.vutbr.cz)
 * @brief Hash table for symtable implementation
 * @date 2020-10-25
 * 
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "symtable.h"
#include "const.h"

/**
 * @brief Hash function, inspiration from http://www.cse.yorku.ca/~oz/hash.html (sdbm version)
 * 
 * @param id 
 * @return unsigned int 
 */
unsigned int hash(char *id) {
    unsigned int h = 0;
    const unsigned char *p;
    for (p = (const unsigned char*)id; *p != '\0'; p++) {
        h = 65599*h + *p;
    }
    return h%TABLE_MAX_SIZE;
}

void tableInit(TablePtr table) {
    for (int i = 0; i < TABLE_MAX_SIZE; i++) {
        table[i] = NULL;
    }
}

bool tableInsert(Item *i, TablePtr table) {
    if (i == NULL) return false;
    int index  = hash(i->id);
    i->next = table[index];
    table[index] = i;
    return true;
}

Item *tableLookup(char *id, TablePtr table) {    
    int index = hash(id);
    Item *tmp = table[index];
    
    while (tmp != NULL && strcmp(tmp->id, id) != 0) {
        tmp = tmp->next;
    }
    return tmp;
}

Item *tableDelete(char *id, TablePtr table) {
    int index = hash(id);
    Item *tmp = table[index];
    Item *prev = table[index];

    while (tmp != NULL && strcmp(tmp->id, id) != 0) {
        prev = tmp;
        tmp = tmp->next;
    }
    if (tmp == NULL) return NULL;
    if (prev == NULL) {
        // Deleting first element
        table[index] = tmp->next;
    }
    else {
        // Deleting nested element
        prev->next = tmp->next;
    }
    return tmp;
}

void tableDispose(TablePtr table) {
    for (int i = 0; i < TABLE_MAX_SIZE; i++) {
        if (table[i] != NULL) {
            while (table[i] != NULL) {
                Item *tmp = table[i];
                table[i] = table[i]->next;
                if (tmp->type == HASH_FUNC) {
                    if (tmp->content.func.params != NULL) {
                        free(tmp->content.func.params);
                    }
                    if (tmp->content.func.retTypes != NULL) {
                        free(tmp->content.func.retTypes);
                    }
                }
                free(tmp);
            }
        }
    }
}