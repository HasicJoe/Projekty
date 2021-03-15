/**
 * @file symtable.h
 * @author Jonáš Tichý (xtichy29@stud.fit.vutbr.cz)
 * @brief Hash table for symtable implementation
 * @date 2020-10-25
 * 
 */
#include <stdint.h>
#include "scanner.h"

typedef enum {HASH_VAR, HASH_FUNC} HashType;

struct ItemVar {
    TokenType dataType;
    union {
        char *string;          
        int64_t integer;           
        double float64; 
    }value;
};

struct ItemFunc {
    TokenType *params;
    int paramsCnt;
    TokenType *retTypes;
    int retTypesCnt;
};

typedef struct mItem{
    char *id;
    HashType type;
    union {
        struct ItemVar var;
        struct ItemFunc func;
    }content;
    struct mItem *next;
} Item;

typedef struct mItem** TablePtr;

unsigned int hash(char *id);
void tableInit(TablePtr);
bool tableInsert(Item *i, TablePtr);
Item *tableDelete(char *id, TablePtr);
Item *tableLookup(char *id, TablePtr);
void tableDispose(TablePtr);