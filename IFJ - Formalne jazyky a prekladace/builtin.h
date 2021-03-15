/**
 * @file builtin.h
 * @author Jonáš Tichý (xtichy29@stud.fit.vutbr.cz)
 * @brief Support module for built-in function analysis
 * @date 2020-12-01
 * 
 */
#include "scanner.h"
#include "list.h"

struct builtInFunc {
    char id[10];
    int retTypesCnt;
    int paramCnt;
    TokenType retTypes[2];
    TokenType paramTypes[3];
};

bool isBuiltin(char *id);
int builtinCheckArgs(char *id, Token **args, int argsCnt, tList *);
TokenType builtinGetRetType(char *id, int pos);
int builtinGetRetCnt(char *id);