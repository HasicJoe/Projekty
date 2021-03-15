/**
 * @file semantics.c
 * @author Jonáš Tichý (xtichy29@stud.fit.vutbr.cz)
 * @author Miroslav Štěpánek (xstepa68@stud.fit.vutbr.cz)
 * @brief Semantic analysis using AST traversal
 * @date 2020-11-26
 * 
 */

#include "semantics.h"
#include "builtin.h"

// Prototypes outside of header file to hide inner functions from the rest of the project
Item *tableListSearchVar(char *, tList *);
int checkExpression(ASTNode *, tList *, TokenType *, bool, bool);
int bodySemantics(ASTNode *, tList *, char *, bool *);
int assignSemantics(ASTNode *, tList *);
int declareSemantics(ASTNode *, tList *);
int funcCallSemantics(ASTNode *, tList *, bool);

/**
 * @brief Find variable in table list, starting from First table (bottom most scope)
 * 
 * @param id 
 * @param tableList 
 * @return Item* 
 */
Item *tableListSearchVar(char *id, tList *tableList) {
    tableList->Act = tableList->First;

    while(tableList->Act != NULL) {
        Item *tmp = tableLookup(id, (TablePtr)tableList->Act->data);
        if (tmp != NULL && tmp->type == HASH_VAR) {
            return tmp;
            break;
        }
        tableList->Act = tableList->Act->rptr;
    }
    return NULL;
}

/**
 * @brief Check expression validity
 * 
 * @param root
 * @param tableList
 * @param tokenType
 * @param isRoot
 * @param strAllow
 * @return int
 */
int checkExpression(ASTNode *root, tList *tableList, TokenType *tokenType, bool isRoot, bool strAllow) {

    // Literal or identifier
    if (root->nodeType == AST_LIT) {
        TokenType litType = TOKEN_TYPE_EMPTY;

        // Identifier
        if (root->content.lit.token->type == TOKEN_TYPE_IDENTIFIER) {
            if (tableListSearchVar(root->content.lit.token->attribute.string, tableList) != NULL) {
                Item *tmp = tableLookup(root->content.lit.token->attribute.string, (TablePtr)tableList->Act->data);
                litType = tmp->content.var.dataType;
            } else {
                return SEMANTIC_DEFINE_ERR;
            }

        // Literal
        } else {
            litType = root->content.lit.token->type;
        }

        // Data type check
        if (litType == TOKEN_TYPE_STRING && !strAllow) {
            return SEMANTIC_TYPECOMPAT_ERR;
        } else if (*tokenType == TOKEN_TYPE_EMPTY) {
            *tokenType = litType;
        } else if (*tokenType != litType) {
            return SEMANTIC_TYPECOMPAT_ERR;
        }

    // Node
    } else {

        // Operators not allowed in string expression
        if (root->content.expr.exprType == EXPR_SUB || root->content.expr.exprType == EXPR_MUL || root->content.expr.exprType == EXPR_DIV) {
            strAllow = false;
        }

        // Division by zero check
        if (root->content.expr.exprType == EXPR_DIV && root->content.expr.content.binaryOp.rChild->nodeType == AST_LIT) {
            if (root->content.expr.content.binaryOp.rChild->content.lit.token->type == TOKEN_TYPE_STRING) {
                return SEMANTIC_TYPECOMPAT_ERR;
            } else if (root->content.expr.content.binaryOp.rChild->content.lit.token->type == TOKEN_TYPE_INTEGER) {
                if (root->content.expr.content.binaryOp.rChild->content.lit.token->attribute.integer == 0) {
                    return SEMANTIC_DIVZERO_ERR;
                }
            } else if (root->content.expr.content.binaryOp.rChild->content.lit.token->type == TOKEN_TYPE_DOUBLE) {
                if (root->content.expr.content.binaryOp.rChild->content.lit.token->attribute.float64 == 0.0) {
                    return SEMANTIC_DIVZERO_ERR;
                }
            }
            int retVal = checkExpression(root->content.expr.content.binaryOp.lChild, tableList, tokenType, false, strAllow);
            if (retVal != 0) {
                return retVal;
            }
            retVal = checkExpression(root->content.expr.content.binaryOp.rChild, tableList, tokenType, false, strAllow);
            if (retVal != 0) {
                return retVal;
            }

        // Relation operator check
        } else if (root->content.expr.exprType >= EXPR_GT && root->content.expr.exprType <= EXPR_NEQ) {
            if (!isRoot) {
                return SEMANTIC_TYPECOMPAT_ERR;
            }
            int retVal = checkExpression(root->content.expr.content.binaryOp.lChild, tableList, tokenType, false, strAllow);
            if (retVal != 0) {
                return retVal;
            }
            retVal = checkExpression(root->content.expr.content.binaryOp.rChild, tableList, tokenType, false, strAllow);
            if (retVal != 0) {
                return retVal;
            }
            *tokenType = TOKEN_TYPE_BOOL;

        // Descend into expression AST
        } else {
            int retVal = checkExpression(root->content.expr.content.binaryOp.lChild, tableList, tokenType, false, strAllow);
            if (retVal != 0) {
                return retVal;
            }
            retVal = checkExpression(root->content.expr.content.binaryOp.rChild, tableList, tokenType, false, strAllow);
            if (retVal != 0) {
                return retVal;
            }
        }
    }
    return 0;
}

/**
 * @brief Semantics interface function handling all semantic checks and returning corresponding return codes to caller
 * 
 * @param root Root AST Node (prog node)
 * @return int Return type as in error.h
 */
int checkSemantics(ASTNode *root) {
    root->content.prog.table = malloc(sizeof(Item*) * TABLE_MAX_SIZE);
    if (root->content.prog.table == NULL) {
        return INTERNAL_ERR;
    }
    TablePtr global = root->content.prog.table;
    tableInit(global);

    // First AST tree pass to fill in function names
    for (int i = 0; i < root->content.prog.bodyCnt; i++) {
        ASTNode *currentFunc = root->content.prog.body[i];
        if (tableLookup(currentFunc->content.func.ID->attribute.string, global) != NULL ||
            isBuiltin(currentFunc->content.func.ID->attribute.string)) {
            // Redefinition of function
            return SEMANTIC_DEFINE_ERR;
        }

        Item *new = malloc(sizeof(Item));
        if (new == NULL) {
            return INTERNAL_ERR;
        }
        new->type = HASH_FUNC;
        new->id = currentFunc->content.func.ID->attribute.string;
        new->content.func.paramsCnt = currentFunc->content.func.paramsCnt;
        new->content.func.retTypesCnt = currentFunc->content.func.retTypesCnt;
        new->content.func.params = malloc(sizeof(TokenAttribute) * new->content.func.paramsCnt);
        new->content.func.retTypes = malloc(sizeof(TokenAttribute) * new->content.func.retTypesCnt);
        if (new->content.func.params == NULL && new->content.func.paramsCnt != 0) return INTERNAL_ERR;
        if (new->content.func.retTypes == NULL && new->content.func.retTypesCnt != 0) return INTERNAL_ERR;
        
        // Params
        for (int i = 0; i < currentFunc->content.func.paramsCnt; i++) {
            // -23 to shift from keyword to datatype
            new->content.func.params[i] = currentFunc->content.func.params[i]->content.funcParam.type->type - 23;
        }

        // RetTypes
        for (int i = 0; i < currentFunc->content.func.retTypesCnt; i++) {
            // -23 to shift from keyword to datatype
            new->content.func.retTypes[i] = currentFunc->content.func.retTypes[i]->type - 23;
        }
        tableInsert(new, global);
    }
    tList *tableList = malloc(sizeof(tList));
    if (tableList == NULL) return INTERNAL_ERR;
    DLInitList(tableList);

    DLInsertFirst(tableList, global);

    // Loop through functions
    for (int i = 0; i < root->content.prog.bodyCnt; i++) {
        ASTNode *currentFunc = root->content.prog.body[i];
        currentFunc->content.func.table = malloc(sizeof(Item*) * TABLE_MAX_SIZE);
        if (currentFunc->content.func.table == NULL) {
            return INTERNAL_ERR;
        }
        TablePtr currentTable = currentFunc->content.func.table;
        tableInit(currentTable);
        DLInsertFirst(tableList, currentTable);

        for (int y = 0; y < currentFunc->content.func.paramsCnt; y++) {
            Item *new = malloc(sizeof(Item));
            if (new == NULL) {
                return INTERNAL_ERR;
            }
            new->type = HASH_VAR;
            new->id = currentFunc->content.func.params[y]->content.funcParam.ID->attribute.string;
            new->content.var.dataType = currentFunc->content.func.params[y]->content.funcParam.type->type - 23;
            tableInsert(new, currentTable);
        }
        
        bool hasReturn = false;
        int retVal = bodySemantics(currentFunc, tableList, currentFunc->content.func.ID->attribute.string, &hasReturn);
        if (retVal != 0) {
            DLDisposeSafe(tableList);
            free(tableList);
            return retVal;
        }

        if (currentFunc->content.func.retTypesCnt != 0 && !hasReturn) {
            DLDisposeSafe(tableList);
            free(tableList);
            return SEMANTIC_PARAM_OR_RET_ERR;
        }
        DLDeleteFirst(tableList);
    }

    // Check if main present and if with correct parameters
    Item *mainFunc = tableLookup("main", (TablePtr)tableList->First->data);
    if (mainFunc != NULL) {
        if (mainFunc->content.func.paramsCnt != 0 || mainFunc->content.func.retTypesCnt != 0) {
            DLDisposeSafe(tableList);
            free(tableList);
            return SEMANTIC_PARAM_OR_RET_ERR;
        }
    }
    else {
        DLDisposeSafe(tableList);
        free(tableList);
        return SEMANTIC_DEFINE_ERR;
    }

    DLDisposeSafe(tableList);
    free(tableList);
    return 0;
}

/**
 * @brief Function to loop through body and perform semantic analysis on it's statements
 * 
 * @param root 
 * @param tableList 
 * @param parentFunc 
 * @param hasReturn 
 * @return int 
 */
int bodySemantics(ASTNode *root, tList *tableList, char *parentFunc, bool *hasReturn) {
    ASTNode **body = NULL;
    int bodyCnt = 0;
    ASTNode **elseBody = NULL;
    int elseBodyCnt = 0;
    switch (root->nodeType) {
        case AST_FUNC:
            body = root->content.func.body;
            bodyCnt = root->content.func.bodyCnt;
            break;
        case AST_IF:
            body = root->content.If.body;
            bodyCnt = root->content.If.bodyCnt;
            elseBody = root->content.If.elseBody;
            elseBodyCnt = root->content.If.elseBodyCnt;
            break;
        case AST_FOR:
            body = root->content.For.body;
            bodyCnt = root->content.For.bodyCnt;
            break;
        default:
            break;
    }

    for (int i = 0; i < bodyCnt + elseBodyCnt; i++) {
        ASTNode *currentStatement = NULL;
        if (i == bodyCnt) {
            // Switch to elseTable
            if (root->nodeType == AST_IF) {
                DLDeleteFirst(tableList);
                DLInsertFirst(tableList, root->content.If.elseTable);
            }
        }
        if (i < bodyCnt) {
            currentStatement = body[i];
        }
        else {
            currentStatement = elseBody[i - bodyCnt];
        }

        switch (currentStatement->nodeType) {
            case AST_DEC: {
                int retVal = declareSemantics(currentStatement, tableList);
                if (retVal != 0) return retVal;
                break;
            } 
            case AST_ASSIGN: {
                int retVal = assignSemantics(currentStatement, tableList);
                if (retVal != 0) return retVal;
                break;
            }
            case AST_FUNCCALL: {
                int retVal = funcCallSemantics(currentStatement, tableList, true);
                if (retVal != 0) return retVal;
                break;
            }
            case AST_IF: {
                currentStatement->content.If.table = malloc(sizeof(Item *) * TABLE_MAX_SIZE);
                if (currentStatement->content.If.table == NULL) {
                    return INTERNAL_ERR;
                }
                if (currentStatement->content.If.elseBodyCnt != 0) {
                    // Init else table
                    currentStatement->content.If.elseTable = malloc(sizeof(Item *) * TABLE_MAX_SIZE);
                    if (currentStatement->content.If.elseTable == NULL) {
                        return INTERNAL_ERR;
                    }
                    tableInit(currentStatement->content.If.elseTable);
                }
                TablePtr newTable = currentStatement->content.If.table;
                tableInit(newTable);
                DLInsertFirst(tableList, newTable);

                TokenType tmp = TOKEN_TYPE_EMPTY;
                int retValCond = checkExpression(currentStatement->content.If.cond, tableList, &tmp, true, true);
                if (retValCond != 0) return retValCond;
                if (tmp != TOKEN_TYPE_BOOL) {
                    return SEMANTIC_TYPECOMPAT_ERR;
                }

                int retVal = bodySemantics(currentStatement, tableList, parentFunc, hasReturn);
                if (retVal != 0) return retVal;
                DLDeleteFirst(tableList);
                break;
            }
            case AST_FOR: {
                currentStatement->content.For.headerTable = malloc(sizeof(Item *) * TABLE_MAX_SIZE);
                TablePtr headerTable = currentStatement->content.For.headerTable;
                if (headerTable == NULL) {
                    return INTERNAL_ERR;
                }
                tableInit(headerTable);
                DLInsertFirst(tableList, headerTable);

                // Declare not null, add to symtable
                if (currentStatement->content.For.forDec != NULL) {
                    int retVal = declareSemantics(currentStatement->content.For.forDec, tableList);
                    if (retVal != 0) return retVal;
                }

                // Check condition
                TokenType tmp = TOKEN_TYPE_EMPTY;
                int retValCond = checkExpression(currentStatement->content.For.forCond, tableList, &tmp, true, true);
                if (retValCond != 0) return retValCond;
                if (tmp != TOKEN_TYPE_BOOL) {
                    return SEMANTIC_TYPECOMPAT_ERR;
                }   
                
                // Inc not null, check assign semantics
                if (currentStatement->content.For.forInc != NULL) {
                    int retVal = assignSemantics(currentStatement->content.For.forInc, tableList);
                    if (retVal != 0) return retVal;
                }

                currentStatement->content.For.table = malloc(sizeof(Item *) * TABLE_MAX_SIZE);
                if (currentStatement->content.For.table == NULL) {
                    return INTERNAL_ERR;
                }
                TablePtr newTable = currentStatement->content.For.table;
                tableInit(newTable);
                DLInsertFirst(tableList, newTable);

                int retVal = bodySemantics(currentStatement, tableList, parentFunc, hasReturn);
                if (retVal != 0) return retVal;
                // Delete both header and body
                DLDeleteFirst(tableList);
                DLDeleteFirst(tableList);
                break;
            }

            case AST_RET: {
                Item *currentFunc = tableLookup(parentFunc, (TablePtr)tableList->Last->data);
                if (currentFunc == NULL) {
                    return SEMANTIC_OTHER_ERR;
                }

                if (currentFunc->content.func.retTypesCnt != currentStatement->content.ret.exprCnt) {
                    return SEMANTIC_PARAM_OR_RET_ERR;
                }
                for (int i = 0; i < currentStatement->content.ret.exprCnt; i++) {
                    ASTNode *currentExpr = currentStatement->content.ret.multiExpr[i];
                    TokenType currentType = TOKEN_TYPE_EMPTY;
                    int retVal = checkExpression(currentExpr, tableList, &currentType, true, true);
                    if (retVal != 0) return retVal;

                    if (currentType != currentFunc->content.func.retTypes[i]) {
                        return SEMANTIC_PARAM_OR_RET_ERR;
                    }
                }
                *hasReturn = true;
                break;
            }  
            
            default:
                break;

        }
    }
    return 0;
}


/**
 * @brief Declare semantic check
 * 
 * @param currentStatement 
 * @param tableList 
 * @return int 
 */
int declareSemantics(ASTNode *currentStatement, tList *tableList) {
    if (tableLookup(currentStatement->content.declare.ID->attribute.string, (TablePtr)tableList->First->data) != NULL || 
        isBuiltin(currentStatement->content.declare.ID->attribute.string)) {
        return SEMANTIC_DEFINE_ERR;
    }
    else {
        Item *new = malloc(sizeof(Item));
        if (new == NULL) {
            return INTERNAL_ERR;
        }
        new->type = HASH_VAR;
        new->id = currentStatement->content.declare.ID->attribute.string;
        new->content.var.dataType = TOKEN_TYPE_EMPTY;
        int retVal = checkExpression(currentStatement->content.declare.expr, tableList, &(new->content.var.dataType), true, true);
        if (retVal != 0) {
            free(new);
            return retVal;
        }
        if (new->content.var.dataType == TOKEN_TYPE_BOOL) {
            free(new);
            return SEMANTIC_DATATYPE_ERR;
        }
        tableInsert(new, tableList->First->data);
    }
    return 0;
}

/**
 * @brief Function call semantic check
 * 
 * @param currentStatement 
 * @param tableList 
 * @param isStatement 
 * @return int 
 */
int funcCallSemantics(ASTNode *currentStatement, tList *tableList, bool isStatement) {
    Token **args = currentStatement->content.funcCall.args;
    char *id = currentStatement->content.funcCall.ID->attribute.string;

    if (tableListSearchVar(id, tableList) != NULL) {
        return SEMANTIC_DEFINE_ERR;
    }

    if (isBuiltin(id)) {
        int retVal = builtinCheckArgs(id, args, currentStatement->content.funcCall.argsCnt, tableList);
        // Function called as a statement, but has return types
        if (isStatement && builtinGetRetCnt(id) != 0) {
            return SEMANTIC_PARAM_OR_RET_ERR;
        }
        return retVal;
    }

    Item *foundItem = tableLookup(id, tableList->Last->data);
    if (foundItem == NULL) {
        return SEMANTIC_DEFINE_ERR;
    }
    else {
        // Function called as a statement, but has return types
        if (isStatement && foundItem->content.func.retTypesCnt != 0) {
            return SEMANTIC_PARAM_OR_RET_ERR;
        }
        // Check if correct number of params, if yes then check type compatability
        if (currentStatement->content.funcCall.argsCnt == foundItem->content.func.paramsCnt) {
            for (int y = 0; y < currentStatement->content.funcCall.argsCnt; y++) {
                if (args[y]->type == TOKEN_TYPE_IDENTIFIER) {
                    Item *found = tableListSearchVar(args[y]->attribute.string, tableList);
                    if (found != NULL) {
                        if (found->content.var.dataType != foundItem->content.func.params[y]) {
                            return SEMANTIC_PARAM_OR_RET_ERR;
                        }
                    }
                    else {
                        return SEMANTIC_DEFINE_ERR;
                    }
                }
                else {
                    if (args[y]->type != foundItem->content.func.params[y]) {
                    // Types not matching, return with error
                        return SEMANTIC_PARAM_OR_RET_ERR;
                    }
                }
            }
        }
        else {
            return SEMANTIC_PARAM_OR_RET_ERR;
        }
    }   
    return 0;
}

/**
 * @brief Assign semantic check
 * 
 * @param currentStatement 
 * @param tableList 
 * @return int 
 */
int assignSemantics(ASTNode *currentStatement, tList *tableList) {
    bool isFuncCall = (currentStatement->content.assign.multiExpr[0]->nodeType == AST_FUNCCALL);
    bool isBuiltinFunc = false;
    Item *foundItem = NULL;
    char *funcID = NULL;
    // Check if funcion exists and is correct semantically
    if (isFuncCall) {
        int retVal = funcCallSemantics(currentStatement->content.assign.multiExpr[0], tableList, false);
        if (retVal != 0) return retVal; 

        funcID = currentStatement->content.assign.multiExpr[0]->content.funcCall.ID->attribute.string;
        isBuiltinFunc = isBuiltin(funcID);

        if (isBuiltinFunc) {
            if (builtinGetRetCnt(funcID) != currentStatement->content.assign.IDCnt) {
                return SEMANTIC_PARAM_OR_RET_ERR;
            }
        }
        else {
            foundItem = tableLookup(funcID, tableList->Last->data);
            if (foundItem->content.func.retTypesCnt != currentStatement->content.assign.IDCnt) {
                return SEMANTIC_PARAM_OR_RET_ERR;
            }
        }
    }
    if (!isFuncCall && (currentStatement->content.assign.IDCnt != currentStatement->content.assign.exprCnt)) {
        return SEMANTIC_OTHER_ERR;
    }
    
    
    for (int p = 0; p < currentStatement->content.assign.IDCnt; p++) {
        TokenType exprType = TOKEN_TYPE_EMPTY;
        if (isFuncCall) {
            if (isBuiltinFunc) {
                exprType = builtinGetRetType(funcID, p);
                if (exprType == TOKEN_TYPE_EMPTY) {
                    return SEMANTIC_PARAM_OR_RET_ERR;
                }
            }
            else {
                if (p >= foundItem->content.func.retTypesCnt) return SEMANTIC_PARAM_OR_RET_ERR;
                exprType = foundItem->content.func.retTypes[p];
            }
        }
        else {
            int retVal = checkExpression(currentStatement->content.assign.multiExpr[p], tableList, &exprType, true, true);
            if (retVal != 0) return retVal;
        }

        if (strcmp(currentStatement->content.assign.IDs[p]->attribute.string, "_") != 0) {
            Item *found = tableListSearchVar(currentStatement->content.assign.IDs[p]->attribute.string, tableList);
            if (found == NULL) {
                return SEMANTIC_DEFINE_ERR;
            }
            if (found->content.var.dataType != exprType) {
                if (isFuncCall) return SEMANTIC_PARAM_OR_RET_ERR;
                else return SEMANTIC_OTHER_ERR;
            }
        }
    }
    return 0;
}