/**
 * @file builtin.c
 * @author Jonáš Tichý (xtichy29@stud.fit.vutbr.cz)
 * @brief Support module for built-in function analysis
 * @date 2020-12-01
 * 
 */
#include "builtin.h"
#include "stdbool.h"
#include "error.h"
#include "symtable.h"

struct builtInFunc inputs = {.id = {"inputs"}, .paramCnt = 0, .retTypesCnt = 2, .retTypes = {TOKEN_TYPE_STRING, TOKEN_TYPE_INTEGER}};
struct builtInFunc inputi = {.id = {"inputi"}, .paramCnt = 0, .retTypesCnt = 2, .retTypes = {TOKEN_TYPE_INTEGER, TOKEN_TYPE_INTEGER}};
struct builtInFunc inputf = {.id = {"inputf"}, .paramCnt = 0, .retTypesCnt = 2, .retTypes = {TOKEN_TYPE_DOUBLE, TOKEN_TYPE_INTEGER}};
struct builtInFunc print = {.id = {"print"}, .paramCnt = -1, .retTypesCnt = 0};
struct builtInFunc int2float = {.id = {"int2float"}, .paramCnt = 1,.paramTypes = {TOKEN_TYPE_INTEGER}, .retTypesCnt = 1, .retTypes = {TOKEN_TYPE_DOUBLE}};
struct builtInFunc float2int = {.id = {"float2int"}, .paramCnt = 1,.paramTypes = {TOKEN_TYPE_DOUBLE}, .retTypesCnt = 1, .retTypes = {TOKEN_TYPE_INTEGER}};
struct builtInFunc len = {.id = {"len"}, .paramCnt = 1,.paramTypes = {TOKEN_TYPE_STRING}, .retTypesCnt = 1, .retTypes = {TOKEN_TYPE_INTEGER}};
struct builtInFunc substr = {.id = {"substr"}, .paramCnt = 3,.paramTypes = {TOKEN_TYPE_STRING, TOKEN_TYPE_INTEGER, TOKEN_TYPE_INTEGER}, .retTypesCnt = 2, .retTypes = {TOKEN_TYPE_STRING, TOKEN_TYPE_INTEGER}};
struct builtInFunc ord = {.id = {"ord"}, .paramCnt = 2,.paramTypes = {TOKEN_TYPE_STRING, TOKEN_TYPE_INTEGER}, .retTypesCnt = 2, .retTypes = {TOKEN_TYPE_INTEGER, TOKEN_TYPE_INTEGER}};
struct builtInFunc chr = {.id = {"chr"}, .paramCnt = 1,.paramTypes = {TOKEN_TYPE_INTEGER}, .retTypesCnt = 2, .retTypes = {TOKEN_TYPE_STRING, TOKEN_TYPE_INTEGER}};


struct builtInFunc *findFunc(char *id) {
    struct builtInFunc *functions[10] = {&inputs, &inputi, &inputf, &print, &int2float, &float2int, &len, &substr, &ord, &chr};
    for (int i = 0; i < 10; i++) {
        if (strcmp(id, functions[i]->id) == 0) {
            return functions[i];
        }
    }
    return NULL;
}

bool isBuiltin(char *id) {
    if (findFunc(id) != NULL) return true;
    return false;
}

int builtinCheckArgs(char *id, Token **args, int argsCnt, tList *tableList) {
    struct builtInFunc *func = findFunc(id);
    if (func == NULL) return SEMANTIC_DEFINE_ERR;

    if (func->paramCnt == argsCnt || func->paramCnt == -1) {
        for (int p = 0; p < argsCnt; p++) {
            if (args[p]->type == TOKEN_TYPE_IDENTIFIER) {
                tableList->Act = tableList->First;
                while (tableList->Act != NULL) {
                    Item *tmp = tableLookup(args[p]->attribute.string, (TablePtr)tableList->Act->data);
                    if (tmp != NULL) {
                        if (tmp->content.var.dataType == func->paramTypes[p] || func->paramCnt == -1) {
                            break;
                        }
                        else {
                            return SEMANTIC_PARAM_OR_RET_ERR;
                        }
                    }
                    tableList->Act = tableList->Act->rptr;
                }
                if (tableList->Act == NULL) {
                    return SEMANTIC_DEFINE_ERR;
                }
            }
            else {
                if (func->paramTypes[p] != args[p]->type && func->paramCnt != -1) {
                    return SEMANTIC_PARAM_OR_RET_ERR;
                } 
            }
        }
        return 0;
    }
    else {
        return SEMANTIC_PARAM_OR_RET_ERR;
    }
   
    return SEMANTIC_PARAM_OR_RET_ERR;
}

TokenType builtinGetRetType(char *id, int pos) {
    struct builtInFunc *func = findFunc(id);
    if (func == NULL) return TOKEN_TYPE_EMPTY;

    if (pos >= func->retTypesCnt) return TOKEN_TYPE_EMPTY;
    return func->retTypes[pos];
}

int builtinGetRetCnt(char *id) {
    return findFunc(id)->retTypesCnt;
}