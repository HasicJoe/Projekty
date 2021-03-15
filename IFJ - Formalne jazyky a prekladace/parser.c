/**
 * @file parser.c
 * @author Jonáš Tichý (xtichy29@stud.fit.vutbr.cz)
 * @author Miroslav Štěpánek (xstepa68@stud.fit.vutbr.cz)
 * @brief Parser providing syntax and AST construction functions
 * @date 2020-10-22
 */

#include <stdio.h>
#include <stdbool.h>
#include "list.h"
#include "parser.h"
#include "scanner.h"
#include "error.h"
#include "string.h"
#include "const.h"
#include "builtin.h"

/**
 * @brief Recursive descent function to perform syntax analysis using grammar from grammar.h
 * 
 * @param currentNode Root node pointer
 * @return true
 * @return false 
 */
int recursive(GrammarNode *currentNode, PTNode* ptNode, tList *list) {
    // LEAF
    if (currentNode->type == LEAF) {
        ptNode->parseType = PT_LEAF;
        ptNode->type = LEAF;

        Token *tmp;
        if (list->Act == list->Last) {
            // All list tokens are used, need to read new token
            tmp = malloc(sizeof(Token));
            if (tmp == NULL) {
                //return INTERNAL_ERR;
            }

            int retVal = getToken(tmp);
            // End of token stream, return false
            if (tmp == NULL || retVal == INTERNAL_ERR || retVal == STATUS_FINISH || retVal == LEXICAL_ERR) {
                free(tmp);
                if (retVal == LEXICAL_ERR || retVal == INTERNAL_ERR) {
                    return retVal;
                }
                return SYNTACTIC_ERR;
            }
            DLInsertLast(list, tmp);
            list->Act = list->Last;
        } else if (list->Act == NULL) {
            // List is reset, reading first token
            tmp = (Token*)list->First->data;
            list->Act = list->First;
        } else {
            // Reading from list
            tmp = (Token*)list->Act->rptr->data;
            list->Act = list->Act->rptr;
        } 
        
        ptNode->content.token = tmp;
        if (currentNode->content.TokenType == tmp->type) {
            return SUCCESS;
        }
        else {
            return SYNTACTIC_ERR;
        }
        //return currentNode->content.TokenType == tmp->type;
    }
    
    // NODE
    ptNode->type = NODE;
    ptNode->parseType = currentNode->parseType;
    ptNode->content.branches.arrSize = 0;

    // Potentional expression found
    if (ptNode->parseType == PT_EXPR) {
        tDLElemPtr saveToken = list->Act;
        int retVal = precExpr(ptNode, list);
        if (retVal != 0) {
            list->Act = saveToken;
            return retVal;
        }
        if (list->Act != NULL && list->First != NULL) {
            while (saveToken->rptr != NULL && saveToken->rptr != list->Act) {
                saveToken = saveToken->rptr;
            }
            list->Act = saveToken;
        }
        return SUCCESS;
    }

    // holder pointer to save position in case of rollback
    tDLElemPtr saveToken = list->Act;
    for (int i = 0; i < currentNode->content.branches.optCnt; i++) {
        // Looping through options
        bool succ = true;
        for (int y = 0; y < currentNode->content.branches.optSize[i]; y++) {
            // Alloc new node and increment
            PTNode *new = malloc(sizeof(PTNode));
            new->content.branches.arrSize = 0;
            ptNode->content.branches.arr[y] = new;
            ptNode->content.branches.arrSize++;

            int retVal = recursive(currentNode->content.branches.opts[i][y], new, list);
            if (retVal == SYNTACTIC_ERR) {
                succ = false;
                list->Act = saveToken;
                ptNodeFree(ptNode);
                break;
            } 
            else if (retVal == SUCCESS) {
                succ = true;
            }
            else {
                return retVal;
            }
        }
        if (succ) {
            return SUCCESS;
        }
        ptNode->content.branches.arrSize = 0;
    }
    return SYNTACTIC_ERR;
}

/**
 * @brief Recursive func to free all allocated parse tree nodes
 * 
 * @param ptNode Root node - needed to be free'd after calling this function
 */
void ptNodeFree(PTNode *ptNode) {
    for (int i = 0; i < ptNode->content.branches.arrSize; i++) {
        if (ptNode->content.branches.arr[i]->type == LEAF) {
            free(ptNode->content.branches.arr[i]);
        } else {
            ptNodeFree(ptNode->content.branches.arr[i]);
            if (ptNode->content.branches.arr[i] != NULL) free(ptNode->content.branches.arr[i]);
        }
    }
    ptNode->content.branches.arrSize = 0;
}

/**
 * @brief Syntax analysis interface
 * 
 * @param grammarRoot Grammar node from which to start recursive descend
 * @param ptRoot Parse tree root node
 * @return int
 */
int checkSyntax(GrammarNode *grammarRoot, PTNode *ptRoot, tList *list) {
    if (ptRoot == NULL) return INTERNAL_ERR;

    int retVal = recursive(grammarRoot, ptRoot, list);
    
    if (retVal == SUCCESS && list->Act != list->Last) {
        retVal = SYNTACTIC_ERR;
    }
    return retVal;
}

/**
 * @brief AST construction interface
 * 
 * @param ptRoot Parse tree root node
 * @param astRoot AST root node
 * @return int 
 */
int buildAST(PTNode *ptRoot, ASTNode *astRoot) {
    if (astRoot == NULL) return INTERNAL_ERR;
    astRoot->nodeType = AST_PROG;

    return progNode(ptRoot, astRoot);
}

/**
 * Following functions (progNode to exprNode) provide AST construction
 * using parse tree constructed earlier in syntax analysis phase. 
 * 
 * Main goal of this process is to build accurate representation of source code
 * and remove redundant tokens (parentheses, newlines, keywords, etc.) for faster
 * and easier semantic analysis and code generation process.
 * 
 */
int progNode(PTNode *ptRoot, ASTNode *currentNode) {
    if (ptRoot->parseType != PT_PROG) {
        exit(SYNTACTIC_ERR);
    }
    currentNode->nodeType = AST_PROG;
    currentNode->content.prog.body = NULL;
    currentNode->content.prog.bodyCnt = 0;
    currentNode->content.prog.table = NULL;
    
    // Skip 0 arr branch - multiEol
    // Skip 1 arr branch - package
    if (strcmp(ptRoot->content.branches.arr[3]->content.token->attribute.string, "main") != 0) {
        return SYNTACTIC_ERR;
    }

    int retVal = 0;
    
    int bodyCurrentSize = PROG_BODY_DEF_SIZE;
    currentNode->content.prog.body = malloc(sizeof(ASTNode*) * PROG_BODY_DEF_SIZE);
    if (currentNode->content.prog.body == NULL) {
        return INTERNAL_ERR;
    }

    
    PTNode *progBody = ptRoot->content.branches.arr[5];

    // Prog body
    while (progBody->content.branches.arrSize != 1) {
        // Function ast node creation
        ASTNode *func = malloc(sizeof(ASTNode));
        if (func == NULL) {
            return INTERNAL_ERR;
        }

        currentNode->content.prog.body[currentNode->content.prog.bodyCnt] = func;
        currentNode->content.prog.bodyCnt++;
        retVal = funcNode(progBody->content.branches.arr[1], func);
        if (retVal != 0) {
            return retVal;
        }

        // realloc check
        if (currentNode->content.prog.bodyCnt == bodyCurrentSize) {
            void *tmp = realloc(currentNode->content.prog.body, bodyCurrentSize * sizeof(ASTNode*) * 2);
            if (tmp == NULL) {
                return INTERNAL_ERR;
            }
            currentNode->content.prog.body = tmp;
            bodyCurrentSize = bodyCurrentSize * 2;
        }
        // descending to lower progBody
        progBody = progBody->content.branches.arr[2];
    }

    return 0;
}

int funcNode(PTNode *ptNode, ASTNode *currentNode) {
    currentNode->nodeType = AST_FUNC;
    currentNode->content.func.ID = ptNode->content.branches.arr[1]->content.token;
    currentNode->content.func.body = NULL;
    currentNode->content.func.params = NULL;
    currentNode->content.func.retTypes = NULL;
    currentNode->content.func.table = NULL;
    currentNode->content.func.bodyCnt = 0;
    currentNode->content.func.paramsCnt = 0;
    currentNode->content.func.retTypesCnt = 0;

    // Func Params
    if (ptNode->content.branches.arr[2]->content.branches.arrSize == 3) {

        int paramCurrentSize = FUNC_PARAMS_DEF_SIZE;
        currentNode->content.func.params = malloc(sizeof(ASTNode*) * FUNC_PARAMS_DEF_SIZE);
        if (currentNode->content.func.params == NULL) {
            return INTERNAL_ERR;
        }

        PTNode *params = ptNode->content.branches.arr[2]->content.branches.arr[1];
        while (true) {
            ASTNode *param = malloc(sizeof(ASTNode));
            if (param == NULL) {
                return INTERNAL_ERR;
            }
            param->nodeType = AST_FUNC_PARAM;
            param->content.funcParam.ID = params->content.branches.arr[0]->content.token;
            param->content.funcParam.type = params->content.branches.arr[1]->content.branches.arr[0]->content.token;
            currentNode->content.func.params[currentNode->content.func.paramsCnt] = param;
            currentNode->content.func.paramsCnt++;
            // Last param
            if (params->content.branches.arrSize == 2) {
                break;
            }

            // Realloc check
            if (currentNode->content.func.paramsCnt == paramCurrentSize) {
                void *tmp = realloc(currentNode->content.func.params, paramCurrentSize * sizeof(ASTNode*) * 2);
                if (tmp == NULL) {
                    return INTERNAL_ERR;
                }
                currentNode->content.func.params = tmp;
                paramCurrentSize = paramCurrentSize *2;
            }
            params = params->content.branches.arr[3];
        }
        
        
    }
    // Func return types
    if (ptNode->content.branches.arr[3]->content.branches.arrSize == 3) {
        int retvalCurrentSize = FUNC_RETTYPES_DEF_SIZE;
        currentNode->content.func.retTypes = malloc(sizeof(Token*) * FUNC_RETTYPES_DEF_SIZE);
        if (currentNode->content.func.retTypes == NULL) {
            return INTERNAL_ERR;
        }

        PTNode *retTypes = ptNode->content.branches.arr[3]->content.branches.arr[1];
        while (true) {
            currentNode->content.func.retTypes[currentNode->content.func.retTypesCnt] = retTypes->content.branches.arr[0]->content.branches.arr[0]->content.token;
            currentNode->content.func.retTypesCnt++;

            // Last return type
            if (retTypes->content.branches.arrSize == 1) {
                break;
            }

            // Realloc check
            if (currentNode->content.func.retTypesCnt == retvalCurrentSize) {
                void *tmp = realloc(currentNode->content.func.retTypes, retvalCurrentSize * sizeof(Token*) * 2);
                if (tmp == NULL) {
                    return INTERNAL_ERR;
                }
                currentNode->content.func.retTypes = tmp;
                retvalCurrentSize = retvalCurrentSize *2;
            }
            retTypes = retTypes->content.branches.arr[2];
        }
    }

    // Func body
    if (ptNode->content.branches.arr[6]->content.branches.arrSize != 1) {
        int retVal = 0;
        int bodyCurrentSize = FUNC_BODY_DEF_SIZE;
        currentNode->content.func.body = malloc(sizeof(ASTNode*) * FUNC_BODY_DEF_SIZE);
        if (currentNode->content.func.body == NULL) {
            return INTERNAL_ERR;
        }

        PTNode *body = ptNode->content.branches.arr[6];
        while (body->content.branches.arrSize == 3) {

            ASTNode *node = malloc(sizeof(ASTNode));
            if (node == NULL) {
                return INTERNAL_ERR;
            }

            switch (body->content.branches.arr[0]->content.branches.arr[0]->parseType) {
                case PT_IF:
                    retVal = ifNode(body->content.branches.arr[0]->content.branches.arr[0], node);
                    break;
                case PT_FOR:
                    retVal = forNode(body->content.branches.arr[0]->content.branches.arr[0], node);
                    break;
                case PT_DEFINE:
                    retVal = declareNode(body->content.branches.arr[0]->content.branches.arr[0], node);
                    break;
                case PT_ASSIGN:
                    retVal = assignNode(body->content.branches.arr[0]->content.branches.arr[0], node);
                    break;
                case PT_RET:
                    retVal = retNode(body->content.branches.arr[0]->content.branches.arr[0], node);
                    break;
                case PT_FUNC_CALL:
                    retVal = funcCallNode(body->content.branches.arr[0]->content.branches.arr[0], node);                    
                    break;
                default:
                    body = body->content.branches.arr[2];
                    free(node);
                    continue;
            }

            currentNode->content.func.body[currentNode->content.func.bodyCnt] = node;
            currentNode->content.func.bodyCnt++;

            if (retVal != 0) {
                return retVal;
            }

            // Realloc check
            if (currentNode->content.func.bodyCnt == bodyCurrentSize) {
                void *tmp = realloc(currentNode->content.func.body, bodyCurrentSize * sizeof(ASTNode*) * 2);
                if (tmp == NULL) {
                    return INTERNAL_ERR;
                }
                currentNode->content.func.body = tmp;
                bodyCurrentSize = bodyCurrentSize * 2;
            }
            body = body->content.branches.arr[2];
        }
        
    }
    return 0;
}

int funcCallNode(PTNode *ptNode, ASTNode *currentNode) {
    currentNode->nodeType = AST_FUNCCALL;
    currentNode->content.funcCall.ID = ptNode->content.branches.arr[0]->content.token;
    currentNode->content.funcCall.argsCnt = 0;
    currentNode->content.funcCall.args = NULL;
    // FuncCall args
    if (ptNode->content.branches.arrSize == 4) {

        int funcCallCurrentSize = FUNCCALL_ARGS_DEF_SIZE;
        currentNode->content.funcCall.args = malloc(sizeof(ASTNode*) * FUNCCALL_ARGS_DEF_SIZE);
        if (currentNode->content.funcCall.args == NULL) {
            return INTERNAL_ERR;
        }

        PTNode *funcArgs = ptNode->content.branches.arr[2];
        while (true) {  
            currentNode->content.funcCall.args[currentNode->content.funcCall.argsCnt] = funcArgs->content.branches.arr[0]->content.branches.arr[0]->content.token;
            currentNode->content.funcCall.argsCnt++;

            // Last argument
            if (funcArgs->content.branches.arrSize != 3) {
                break;
            }

            // realloc check
            if (currentNode->content.funcCall.argsCnt == funcCallCurrentSize) {
                void *tmp = realloc(currentNode->content.funcCall.args, sizeof(ASTNode*) * funcCallCurrentSize * 2);
                if (tmp == NULL) {
                    return INTERNAL_ERR;
                }
                currentNode->content.funcCall.args = tmp;
                funcCallCurrentSize = funcCallCurrentSize * 2;
            }
            funcArgs = funcArgs->content.branches.arr[2];
        }
    }
    return 0;
}

int assignNode(PTNode *ptNode, ASTNode *currentNode) {
    currentNode->nodeType = AST_ASSIGN;
    currentNode->content.assign.IDs = NULL;
    currentNode->content.assign.multiExpr = NULL;
    currentNode->content.assign.IDCnt = 0;
    currentNode->content.assign.exprCnt = 0;
    int idsCurrentSize = ASSIGN_IDS_SIZE;
    int exprCurrentSize = ASSIGN_EXPR_SIZE;
    int retVal = 0;

    currentNode->content.assign.IDs = malloc(sizeof(Token*) * ASSIGN_IDS_SIZE);
    if (currentNode->content.assign.IDs == NULL) {
        return INTERNAL_ERR;
    }

    currentNode->content.assign.multiExpr = malloc(sizeof(ASTNode*) * ASSIGN_EXPR_SIZE);
    if (currentNode->content.assign.multiExpr == NULL) {
        return INTERNAL_ERR;
    }

    PTNode *ids = ptNode->content.branches.arr[0];

    // Assign Ids
    while (true) {
        currentNode->content.assign.IDs[currentNode->content.assign.IDCnt] = ids->content.branches.arr[0]->content.token;
        currentNode->content.assign.IDCnt++;

        if (ids->content.branches.arrSize == 1) {
            break;
        }

        // Realloc check
        if (currentNode->content.assign.IDCnt == idsCurrentSize) {
            void *tmp = realloc(currentNode->content.assign.IDs, idsCurrentSize * sizeof(Token*) * 2);
            if (tmp == NULL) {
                return INTERNAL_ERR;
            }
            currentNode->content.assign.IDs = tmp;
            idsCurrentSize = idsCurrentSize * 2;
        }
        ids = ids->content.branches.arr[2];
    }

    // Assign mulExpr
    if (ptNode->content.branches.arr[3]->parseType == PT_MULTIEXPR) {
        PTNode *multiExpr = ptNode->content.branches.arr[3];

        while (true) {
            ASTNode *node = malloc(sizeof(ASTNode));
            if (node == NULL) {
                return INTERNAL_ERR;
            }

            retVal = exprNode(multiExpr->content.branches.arr[0], node);
            currentNode->content.assign.multiExpr[currentNode->content.assign.exprCnt] = node;
            currentNode->content.assign.exprCnt++;
            if (retVal != 0) {
                return retVal;
            }

            if (multiExpr->content.branches.arrSize == 1) {
                break;
            }

            // Realloc check
            if (currentNode->content.assign.exprCnt == exprCurrentSize) {
                void *tmp = realloc(currentNode->content.assign.multiExpr, exprCurrentSize * sizeof(ASTNode*) * 2);
                if (tmp == NULL) {
                    return INTERNAL_ERR;
                }
                currentNode->content.assign.multiExpr = tmp;
                exprCurrentSize = exprCurrentSize * 2;
            }
            multiExpr = multiExpr->content.branches.arr[2];
        }
    } else {
        ASTNode *funcCall = malloc(sizeof(ASTNode));
        if (funcCall == NULL) {
            return INTERNAL_ERR;
        }

        retVal = funcCallNode(ptNode->content.branches.arr[3], funcCall);
        currentNode->content.assign.multiExpr[0] = funcCall;
        currentNode->content.assign.exprCnt = 1;

        if (retVal != 0) {
            return retVal;
        }
    }

    return 0;
}

int declareNode(PTNode *ptNode, ASTNode *currentNode) {
    currentNode->nodeType = AST_DEC;
    currentNode->content.declare.ID = ptNode->content.branches.arr[0]->content.token;
    currentNode->content.declare.expr = NULL;

    ASTNode *node = malloc(sizeof(ASTNode));
    if (node == NULL) {
        return INTERNAL_ERR;
    }
    
    int retVal = exprNode(ptNode->content.branches.arr[3], node);
    currentNode->content.declare.expr = node;
    if (retVal != 0) {
        return retVal;
    }
    
    return 0;
}

int retNode(PTNode *ptNode, ASTNode *currentNode) {
    currentNode->nodeType = AST_RET;
    currentNode->content.ret.multiExpr = NULL;
    currentNode->content.ret.exprCnt = 0;
    int exprCurrentSize = ASSIGN_EXPR_SIZE;
    int retVal = 0;

    if (ptNode->content.branches.arrSize == 2) {

        currentNode->content.ret.multiExpr = malloc(sizeof(ASTNode*) * ASSIGN_EXPR_SIZE);
        if (currentNode->content.ret.multiExpr == NULL) {
            return INTERNAL_ERR;
        }
        
        PTNode *multiExpr = ptNode->content.branches.arr[1];

        while (true) {
            ASTNode *node = malloc(sizeof(ASTNode));
            if (node == NULL) {
                return INTERNAL_ERR;
            }
            retVal = exprNode(multiExpr->content.branches.arr[0], node);
            currentNode->content.ret.multiExpr[currentNode->content.ret.exprCnt] = node;
            currentNode->content.ret.exprCnt++;
            if (retVal != 0) {
                return retVal;
            }

            if (multiExpr->content.branches.arrSize == 1) {
                break;
            }

            // Realloc check
            if (currentNode->content.ret.exprCnt == exprCurrentSize) {
                void *tmp = realloc(currentNode->content.ret.multiExpr, exprCurrentSize * sizeof(ASTNode*) * 2);
                if (tmp == NULL) {
                    return INTERNAL_ERR;
                }
                currentNode->content.ret.multiExpr = tmp;
                exprCurrentSize = exprCurrentSize * 2;
            }
            multiExpr = multiExpr->content.branches.arr[2];
        }
    }
    
    return 0;
}

int ifNode(PTNode *ptNode, ASTNode *currentNode) {
    currentNode->nodeType = AST_IF;
    currentNode->content.If.body = NULL;
    currentNode->content.If.cond = NULL;
    currentNode->content.If.elseBody = NULL;
    currentNode->content.If.table = NULL;
    currentNode->content.If.elseTable = NULL;
    currentNode->content.If.bodyCnt = 0;
    currentNode->content.If.elseBodyCnt = 0;

    int bodyCurrentSize = FUNC_BODY_DEF_SIZE;
    int elseCurrentSize = FUNC_BODY_DEF_SIZE;
    int retVal = 0;

    ASTNode *node = malloc(sizeof(ASTNode));
    if (node == NULL) {
        return INTERNAL_ERR;
    }

    
    // If condition
    retVal = exprNode(ptNode->content.branches.arr[1], node);
    currentNode->content.If.cond = node;
    if (retVal != 0) {
        return retVal;
    }
    
    // If body
    if (ptNode->content.branches.arr[4]->content.branches.arrSize != 1) {

        currentNode->content.If.body = malloc(sizeof(ASTNode*) * FUNC_BODY_DEF_SIZE);
        if (currentNode->content.If.body == NULL) {
            return INTERNAL_ERR;
        }
        
        PTNode *body = ptNode->content.branches.arr[4];
        while (body->content.branches.arrSize == 3) {
            
            ASTNode *node = malloc(sizeof(ASTNode));
            if (node == NULL) {
                return INTERNAL_ERR;
            }
            switch (body->content.branches.arr[0]->content.branches.arr[0]->parseType) {
                case PT_IF:
                    retVal = ifNode(body->content.branches.arr[0]->content.branches.arr[0], node);
                    break;
                case PT_FOR:
                    retVal = forNode(body->content.branches.arr[0]->content.branches.arr[0], node);
                    break;
                case PT_DEFINE:
                    retVal = declareNode(body->content.branches.arr[0]->content.branches.arr[0], node);
                    break;
                case PT_ASSIGN:
                    retVal = assignNode(body->content.branches.arr[0]->content.branches.arr[0], node);
                    break;
                case PT_RET:
                    retVal = retNode(body->content.branches.arr[0]->content.branches.arr[0], node);
                    break;
                case PT_FUNC_CALL:
                    retVal = funcCallNode(body->content.branches.arr[0]->content.branches.arr[0], node);
                    break;
                default:
                    // Eps
                    body = body->content.branches.arr[2];
                    free(node);
                    continue;
            }
            currentNode->content.If.body[currentNode->content.If.bodyCnt] = node;
            currentNode->content.If.bodyCnt++;

            if (retVal != 0) {
                return retVal;
            }

            // Realloc check
            if (currentNode->content.If.bodyCnt == bodyCurrentSize) {
                void *tmp = realloc(currentNode->content.If.body, bodyCurrentSize * sizeof(ASTNode*) * 2);
                if (tmp == NULL) {
                    return INTERNAL_ERR;
                }
                currentNode->content.If.body = tmp;
                bodyCurrentSize = bodyCurrentSize * 2;
            }
            body = body->content.branches.arr[2];
        }
    }

    // Else
    if (ptNode->content.branches.arrSize == 11) {
        currentNode->content.If.elseBody = malloc(sizeof(ASTNode*) * FUNC_BODY_DEF_SIZE);
        if (currentNode->content.If.elseBody == NULL) {
            return INTERNAL_ERR;
        }

        PTNode *elseBody = ptNode->content.branches.arr[9];
        while (elseBody->content.branches.arrSize == 3) {

            ASTNode *node = malloc(sizeof(ASTNode));
            if (node == NULL) {
                return INTERNAL_ERR;
            }
            switch (elseBody->content.branches.arr[0]->content.branches.arr[0]->parseType) {
            case PT_IF:
                retVal = ifNode(elseBody->content.branches.arr[0]->content.branches.arr[0], node);
                break;
            case PT_FOR:
                retVal = forNode(elseBody->content.branches.arr[0]->content.branches.arr[0], node);
                break;
            case PT_DEFINE:
                retVal = declareNode(elseBody->content.branches.arr[0]->content.branches.arr[0], node);
                break;
            case PT_ASSIGN:
                retVal = assignNode(elseBody->content.branches.arr[0]->content.branches.arr[0], node);
                break;
            case PT_RET:
                retVal = retNode(elseBody->content.branches.arr[0]->content.branches.arr[0], node);
                break;
            case PT_FUNC_CALL:
                retVal = funcCallNode(elseBody->content.branches.arr[0]->content.branches.arr[0], node);
                break;
            default:
                // Eps
                elseBody = elseBody->content.branches.arr[2];
                free(node);
                continue;
            }
            currentNode->content.If.elseBody[currentNode->content.If.elseBodyCnt] = node;
            currentNode->content.If.elseBodyCnt++;

            if (retVal != 0) {
                return retVal;
            }

            // Realloc check
            if (currentNode->content.If.elseBodyCnt == elseCurrentSize) {
                void *tmp = realloc(currentNode->content.If.elseBody, elseCurrentSize * sizeof(ASTNode*) * 2);
                if (tmp == NULL) {
                    return INTERNAL_ERR;
                }
                currentNode->content.If.elseBody = tmp;
                elseCurrentSize = elseCurrentSize * 2;
            }
            elseBody = elseBody->content.branches.arr[2];
        }
    }
    return 0;
}

int forNode(PTNode *ptNode, ASTNode *currentNode) {
    currentNode->nodeType = AST_FOR;
    currentNode->content.For.body = NULL;
    currentNode->content.For.forCond = NULL;
    currentNode->content.For.forDec = NULL;
    currentNode->content.For.forInc = NULL;
    currentNode->content.For.table = NULL;
    currentNode->content.For.headerTable = NULL;
    currentNode->content.For.bodyCnt = 0;
    int forCurrentSize = FUNC_BODY_DEF_SIZE;
    int retVal = 0;

    // For definition
    if (ptNode->content.branches.arr[1]->content.branches.arr[0]->parseType == PT_DEFINE) {
        ASTNode *define = malloc(sizeof(ASTNode));
        if (define == NULL) {
            return INTERNAL_ERR;
        }

        retVal = declareNode(ptNode->content.branches.arr[1]->content.branches.arr[0], define);
        currentNode->content.For.forDec = define;
        if (retVal != 0) {
            return retVal;
        }
    }

    ASTNode *cond = malloc(sizeof(ASTNode));
    if (cond == NULL) {
        return INTERNAL_ERR;
    }

    // For condition
    retVal = exprNode(ptNode->content.branches.arr[3], cond);
    currentNode->content.For.forCond = cond;
    if (retVal != 0) {
        return retVal;
    }

    // Inc 
    if (ptNode->content.branches.arr[5]->content.branches.arr[0]->parseType == PT_ASSIGN) {
        ASTNode *assign = malloc(sizeof(ASTNode));
        if (assign == NULL) {
            return INTERNAL_ERR;
        }
        retVal = assignNode(ptNode->content.branches.arr[5]->content.branches.arr[0], assign);
        currentNode->content.For.forInc = assign;
        if (retVal != 0) {
            return retVal;
        }

    }

    // For body
    if (ptNode->content.branches.arr[8]->content.branches.arrSize != 1) {

        currentNode->content.For.body = malloc(sizeof(ASTNode*) * FUNC_BODY_DEF_SIZE);
        if (currentNode->content.For.body == NULL) {
            return INTERNAL_ERR;
        }

        PTNode *forBody = ptNode->content.branches.arr[8];
        while (forBody->content.branches.arrSize == 3) {
            ASTNode *node = malloc(sizeof(ASTNode));
            if (node == NULL) {
                return INTERNAL_ERR;
            }
            switch (forBody->content.branches.arr[0]->content.branches.arr[0]->parseType) {
                case PT_IF:
                    retVal = ifNode(forBody->content.branches.arr[0]->content.branches.arr[0], node);
                    break;
                case PT_FOR:
                    retVal = forNode(forBody->content.branches.arr[0]->content.branches.arr[0], node);
                    break;
                case PT_DEFINE:
                    retVal = declareNode(forBody->content.branches.arr[0]->content.branches.arr[0], node);
                    break;
                case PT_ASSIGN:
                    retVal = assignNode(forBody->content.branches.arr[0]->content.branches.arr[0], node);
                    break;
                case PT_RET:
                    retVal = retNode(forBody->content.branches.arr[0]->content.branches.arr[0], node);
                    break;
                case PT_FUNC_CALL:
                    retVal = funcCallNode(forBody->content.branches.arr[0]->content.branches.arr[0], node);
                    break;
                default:
                    // Eps
                    forBody = forBody->content.branches.arr[2];
                    free(node);
                    continue;
            }
            
            currentNode->content.For.body[currentNode->content.For.bodyCnt] = node;
            currentNode->content.For.bodyCnt++;

            if (retVal != 0) {
                return retVal;
            }

            // Realloc check
            if (currentNode->content.For.bodyCnt == forCurrentSize) {
                void *tmp = realloc(currentNode->content.For.body, forCurrentSize * sizeof(ASTNode*) * 2);
                if (tmp == NULL) {
                    return INTERNAL_ERR;
                }
                currentNode->content.For.body = tmp;
                forCurrentSize = forCurrentSize * 2;
            }
            forBody = forBody->content.branches.arr[2];
        }
    }
    return 0;
}

int exprNode(PTNode *ptNode, ASTNode *currentNode) {
    currentNode->content.expr.content.binaryOp.lChild = NULL;
    currentNode->content.expr.content.binaryOp.rChild = NULL;
    currentNode->nodeType = AST_EXPR;

    int retVal = PTtoAST(ptNode->content.branches.arr[0], currentNode);
    if (retVal != 0) {
        return retVal;
    }
    return 0;
}

/**
 * @brief Converts PT into AST
 * 
 * @param ptNode 
 * @param newExpr 
 * @return int 
 */
int PTtoAST(PTNode *ptNode, ASTNode *newExpr) {

    // ptNode consists of 3 sub ptNodes
    if (ptNode->content.branches.arrSize == 3) {
        // ptNode consists of 2 oprands and 1 operator
        if (ptNode->content.branches.arr[1]->type == LEAF) {

            newExpr->nodeType = AST_EXPR;

            // Get operator type
            switch(ptNode->content.branches.arr[1]->content.token->type) {
                case TOKEN_TYPE_ADD:
                    newExpr->content.expr.exprType = EXPR_ADD;
                    break;
                case TOKEN_TYPE_SUB:
                    newExpr->content.expr.exprType = EXPR_SUB;
                    break;
                case TOKEN_TYPE_MUL:
                    newExpr->content.expr.exprType = EXPR_MUL;
                    break;
                case TOKEN_TYPE_DIV:
                    newExpr->content.expr.exprType = EXPR_DIV;
                    break;
                case TOKEN_TYPE_GT:
                    newExpr->content.expr.exprType = EXPR_GT;
                    break;
                case TOKEN_TYPE_GTE:
                    newExpr->content.expr.exprType = EXPR_GTE;
                    break;
                case TOKEN_TYPE_LT:
                    newExpr->content.expr.exprType = EXPR_LT;
                    break;
                case TOKEN_TYPE_LTE:
                    newExpr->content.expr.exprType = EXPR_LTE;
                    break;
                case TOKEN_TYPE_EQ:
                    newExpr->content.expr.exprType = EXPR_EQ;
                    break;
                case TOKEN_TYPE_NEQ:
                    newExpr->content.expr.exprType = EXPR_NEQ;
                    break;
                default:
                    break;
            }

            // Right child is LEAF
            if (ptNode->content.branches.arr[0]->type == LEAF) {
                ASTNode *lit = malloc(sizeof(ASTNode));
                if (lit == NULL) {
                    free(newExpr);
                    return INTERNAL_ERR;
                }
                lit->nodeType = AST_LIT;
                lit->content.lit.token = ptNode->content.branches.arr[0]->content.token;
                newExpr->content.expr.content.binaryOp.rChild = lit;
            
            // Right child is NODE
            } else {
                ASTNode *node = malloc(sizeof(ASTNode));
                PTtoAST(ptNode->content.branches.arr[0], node);
                newExpr->content.expr.content.binaryOp.rChild = node;
            }

            // Left child is LEAF
            if (ptNode->content.branches.arr[2]->type == LEAF) {
                ASTNode *lit = malloc(sizeof(ASTNode));
                if (lit == NULL) {
                    free(newExpr->content.expr.content.binaryOp.rChild);
                    free(newExpr);
                    return INTERNAL_ERR;
                }
                lit->nodeType = AST_LIT;
                lit->content.lit.token = ptNode->content.branches.arr[2]->content.token;
                newExpr->content.expr.content.binaryOp.lChild = lit;
            
            // Left child is NODE
            } else {
                ASTNode *node = malloc(sizeof(ASTNode));
                PTtoAST(ptNode->content.branches.arr[2], node);
                newExpr->content.expr.content.binaryOp.lChild = node;
            }
        
        // ptNode consists of parentheses and a ptNode
        } else {
            PTtoAST(ptNode->content.branches.arr[1], newExpr);
        }

    // ptNode consists of 1 operand
    } else {
        newExpr->nodeType = AST_LIT;
        newExpr->content.lit.token = ptNode->content.branches.arr[0]->content.token;
    }
    return 0;
}

/**
 * @brief Shifts REDUCTION_MARKER and token onto stack
 * 
 * @param token 
 * @param expList 
 * @return int 
 */
int precShift(Token *token, tList *expList) {
    
    Token *handle = malloc(sizeof(Token));
    if (handle == NULL) {
        return INTERNAL_ERR;
    }
    handle->type = REDUCTION_MARKER;

    EXPNode *expNode = malloc(sizeof(EXPNode));
    if (expNode == NULL) {
        return INTERNAL_ERR;
    }
    expNode->content.token = handle;
    expNode->itemType = LEAF;

    EXPNode *node = malloc(sizeof(EXPNode));
    if (node == NULL) {
        return INTERNAL_ERR;
    }
    node->content.token = token;
    node->itemType = LEAF;

    // Insert the REDUCTION_MARKER after the last LEAF if any is present,
    // otherwise insert REDUCTION_MARKER as the first element
    if (expList->Act == NULL) {
        if (DLInsertFirst(expList, expNode)) {
            return INTERNAL_ERR;
        }
    } else {
        if (DLInsertAct(expList, expNode)) {
            return INTERNAL_ERR;
        }
    }
    
    if (DLInsertLast(expList, node)) {
        return INTERNAL_ERR;
    }
    return 0;
}

/**
 * @brief Gets operator priority
 * relation operators  ->  0
 * +, -  ->  1
 * *, /  ->  2
 * 
 * @param token 
 * @return int 
 */
int getOpPrio(Token *token) {
    if (token->type == TOKEN_TYPE_ADD || token->type == TOKEN_TYPE_SUB) {
        return 1;
    } else if (token->type == TOKEN_TYPE_MUL || token->type == TOKEN_TYPE_DIV) {
        return 2;
    }
    return 0;
}

/**
 * @brief Reduces top elements of the stack and puts them into a new PTNode until REDUCTION_MARKER is found
 * 
 * @param expList 
 * @return int 
 */
int precReduce(tList *expList) {

    PTNode *handleRoot = malloc(sizeof(PTNode));
    if (handleRoot == NULL) {
        return INTERNAL_ERR;
    }
    handleRoot->content.branches.arrSize = 0;
    handleRoot->type = NODE;

    EXPNode *expNode = malloc(sizeof(EXPNode));
    if (expNode == NULL) {
        return INTERNAL_ERR;
    }
    expNode->content.ptNode = handleRoot;
    expNode->itemType = NODE;

    // Add PTNodes and LEAFs into handleRoot until reduction marker is found
    for (int i = 0; ((EXPNode *) expList->Last->data)->content.token->type != REDUCTION_MARKER; i++) {
        if (((EXPNode *) expList->Last->data)->itemType == LEAF) {
            PTNode *node = malloc(sizeof(PTNode));
            if (node == NULL) {
                return INTERNAL_ERR;
            }
            node->type = LEAF;
            node->content.token = ((EXPNode *) expList->Last->data)->content.token;
            handleRoot->content.branches.arr[i] = node;
        } else {
            handleRoot->content.branches.arr[i] = ((EXPNode *) expList->Last->data)->content.ptNode;
        }
        free(((EXPNode *) expList->Last->data));
        handleRoot->content.branches.arrSize++;
        DLDeleteLast(expList);

    }
    free(((EXPNode *) expList->Last->data)->content.token);
    free(((EXPNode *) expList->Last->data));
    DLDeleteLast(expList);    
    if (DLInsertLast(expList, expNode)) {
        return INTERNAL_ERR;
    }
    return 0;
}

/**
 * @brief Finds the last LEAF in the stack if present
 * 
 * @param expList 
 */
void findLastLeaf(tList *expList) {
    expList->Act = expList->First;
    tDLElemPtr tmp = NULL;
    while (expList->Act != NULL) {
        if (((EXPNode *) expList->Act->data)->itemType == LEAF) {
            tmp = expList->Act;
        }
        expList->Act = expList->Act->rptr;
    }
    expList->Act = tmp;
}

/**
 * @brief Decides when to shift and when to reduce expList elements
 * 
 * @param token 
 * @param expList 
 * @return int 
 */
int precTable(Token *token, tList *expList) {

    // Get expList act
    findLastLeaf(expList);

    // Literals and identifiers
    if (isLitOrID(token)) {
        if (expList->Act == NULL || ((EXPNode *) expList->Act->data)->content.token->type != TOKEN_TYPE_RIGHT_PARENTHESE) {
            int retVal = precShift(token, expList);
            if (retVal != 0) {
                return retVal;
            }
            retVal = precReduce(expList);
            if (retVal != 0) {
                return retVal;
            }
        } else {
            return SYNTACTIC_ERR;
        }

    // Operators
    } else if (isOperator(token)) {
        if (expList->Act == NULL)  {
            int retVal = precShift(token, expList);
            if (retVal != 0) {
                return retVal;
            }

        // Add, Sub
        } else if (token->type == TOKEN_TYPE_ADD || token->type == TOKEN_TYPE_SUB) {
            if ((((EXPNode *) expList->Act->data)->content.token->type >= TOKEN_TYPE_ADD && ((EXPNode *) expList->Act->data)->content.token->type <= TOKEN_TYPE_DIV)
                || ((EXPNode *) expList->Act->data)->content.token->type == TOKEN_TYPE_RIGHT_PARENTHESE) {

                int retVal = precReduce(expList);
                if (retVal != 0) {
                    return retVal;
                }
                findLastLeaf(expList);
            }
            int retVal = precShift(token, expList);
            if (retVal != 0) {
                return retVal;
            }

        // Mul, Div
        } else if (token->type == TOKEN_TYPE_MUL || token->type == TOKEN_TYPE_DIV) {
            if (((EXPNode *) expList->Act->data)->content.token->type == TOKEN_TYPE_MUL || ((EXPNode *) expList->Act->data)->content.token->type == TOKEN_TYPE_DIV
                || ((EXPNode *) expList->Act->data)->content.token->type == TOKEN_TYPE_RIGHT_PARENTHESE) {
                
                int retVal = precReduce(expList);
                if (retVal != 0) {
                    return retVal;
                }
                findLastLeaf(expList);
            }
            int retVal = precShift(token, expList);
            if (retVal != 0) {
                return retVal;
            }

        // Relation operators
        } else {
            if (((EXPNode *) expList->Act->data)->content.token->type >= TOKEN_TYPE_LT && ((EXPNode *) expList->Act->data)->content.token->type <= TOKEN_TYPE_NEQ) {
                return SYNTACTIC_ERR;
            } else if ((((EXPNode *) expList->Act->data)->content.token->type >= TOKEN_TYPE_ADD && ((EXPNode *) expList->Act->data)->content.token->type <= TOKEN_TYPE_DIV)
                || ((EXPNode *) expList->Act->data)->content.token->type == TOKEN_TYPE_RIGHT_PARENTHESE) {
                
                int retVal = precReduce(expList);
                if (retVal != 0) {
                    return retVal;
                }
                findLastLeaf(expList);
            }
            int retVal = precShift(token, expList);
            if (retVal != 0) {
                return retVal;
            }
        }

    // Left parenthese
    } else if (token->type == TOKEN_TYPE_LEFT_PARENTHESE) {
        if (expList->Act != NULL && ((EXPNode *) expList->Act->data)->content.token->type == TOKEN_TYPE_RIGHT_PARENTHESE) {
            return SYNTACTIC_ERR;
        }
        int retVal = precShift(token, expList);
        if (retVal != 0) {
            return retVal;
        }

    // Right parenthese
    } else if (token->type == TOKEN_TYPE_RIGHT_PARENTHESE) {
        if (expList->Act == NULL) {
            return SYNTACTIC_ERR;
        } else if (((EXPNode *) expList->Act->data)->content.token->type == TOKEN_TYPE_LEFT_PARENTHESE) {
            EXPNode *expNode = malloc(sizeof(EXPNode));
            if (expNode == NULL) {
                return INTERNAL_ERR;
            }
            expNode->content.token = token;
            expNode->itemType = LEAF;
            if (DLInsertLast(expList, expNode)) {
                return INTERNAL_ERR;
            }
            int retVal = precReduce(expList);
            if (retVal != 0) {
                return retVal;
            }

        } else {
            findLastLeaf(expList);
            while(isOperator(((EXPNode *) expList->Act->data)->content.token)) {
                int retVal = precReduce(expList);
                if (retVal != 0) {
                    return retVal;
                }
                findLastLeaf(expList);
                if (expList->Act == NULL) {
                    return SYNTACTIC_ERR;
                }
            }
            EXPNode *expNode = malloc(sizeof(EXPNode));
            if (expNode == NULL) {
                return INTERNAL_ERR;
            }
            expNode->content.token = token;
            expNode->itemType = LEAF;
            if (DLInsertLast(expList, expNode)) {
                return INTERNAL_ERR;
            }
            int retVal = precReduce(expList);
            if (retVal != 0) {
                return retVal;
            }
        }

    // EOL or token that is not a part of the expression
    } else {
        if (expList->First == NULL) {
            return SYNTACTIC_ERR;
        }
        if ((expList->Act != NULL && (((EXPNode *) expList->Last->data)->itemType == NODE || ((EXPNode *) expList->Act->data)->content.token->type == TOKEN_TYPE_RIGHT_PARENTHESE))
            || (expList->First == expList->Last && ((EXPNode *) expList->First->data)->itemType == NODE)) {

            // If only one element remains - Success
            if (expList->First == expList->Last) {
                return PRECEDENCE_END;
            }

            // Reduce elements until only one remains
            while (expList->First != expList->Last) {
                int retVal = precReduce(expList);
                if (retVal != 0) {
                    return retVal;
                }
            }

            // Success
            return PRECEDENCE_END;

        } else if (token->type == TOKEN_TYPE_EOL) {
            // EOL allowed only after operator or left parenthese
            if ((expList->Act != NULL && (!isOperator(((EXPNode *) expList->Act->data)->content.token)
                && ((EXPNode *) expList->Act->data)->content.token->type != TOKEN_TYPE_LEFT_PARENTHESE)) || expList->First == NULL) {

                return SYNTACTIC_ERR;
            }
        } else {
            return SYNTACTIC_ERR;
        }
    }
    return 0;
}

/**
 * @brief Finds out if token is an operator
 * 
 * @param token 
 * @return true 
 * @return false 
 */
bool isOperator(Token *token) {
    return (token->type >= TOKEN_TYPE_ADD && token->type <= TOKEN_TYPE_DIV)
        || (token->type >= TOKEN_TYPE_LT && token->type <= TOKEN_TYPE_NEQ);
}

/**
 * @brief Finds out if token is a literal or an identifier
 * 
 * @param token 
 * @return true 
 * @return false 
 */
bool isLitOrID(Token *token) {
    return token->type == TOKEN_TYPE_IDENTIFIER || token->type == TOKEN_TYPE_INTEGER
        || token->type == TOKEN_TYPE_DOUBLE || token->type == TOKEN_TYPE_STRING;
}

/**
 * @brief Frees memory which was allocated by precedence
 * 
 * @param expList 
 */
void freePrec(tList *expList) {
    tDLElemPtr tmp = expList->First;
    while (tmp != NULL) {
        if (((EXPNode *) tmp->data)->itemType == NODE) {
            ptNodeFree(((EXPNode *) tmp->data)->content.ptNode);
            free(((EXPNode *) tmp->data)->content.ptNode);
        } else if (((EXPNode *) tmp->data)->content.token->type == REDUCTION_MARKER) {
            free(((EXPNode *) tmp->data)->content.token);
        }
        free(((EXPNode *) tmp->data));
        tmp = tmp->rptr;
    }
    DLDisposeSafe(expList);
    free(expList);   
}

/**
 * @brief Creates expression parse tree, based on precedence table
 * 
 * @param ptNode 
 * @param list 
 * @return int 
 */
int precExpr(PTNode* ptNode, tList *list) {
    
    tList *expList = malloc(sizeof(tList));
    if (expList == NULL) {
        return INTERNAL_ERR;
    }
    DLInitList(expList);

    // Changes based on what is syntactically allowed 
    int allowedFlag = 0;
    int parCount = 0;

    while (true) {
        Token *token;

        // All list tokens are used, need to read new token
        if (list->Act == list->Last) {
            token = malloc(sizeof(Token));
            int retVal = getToken(token);
            if (token == NULL || retVal == INTERNAL_ERR || retVal == STATUS_FINISH || retVal == LEXICAL_ERR) {
                free(token);
                freePrec(expList);
                if (retVal == LEXICAL_ERR || retVal == INTERNAL_ERR) {
                    return retVal;
                }
                return SYNTACTIC_ERR;
            }
            if (DLInsertLast(list, token)) {
                return INTERNAL_ERR;
            }
            list->Act = list->Last;

        // List is reset, reading first token
        } else if (list->Act == NULL) {
            token = list->First->data;
            list->Act = list->First;

        // Reading from list
        } else {
            token = list->Act->rptr->data;
            list->Act = list->Act->rptr;
        }

        // Check syntax of the expression
        // Literal or identifier
        if (isLitOrID(token)) {
            if (allowedFlag == 0) {
                allowedFlag = 1;
            } else {
                freePrec(expList);
                return SYNTACTIC_ERR;
            }
        // Operator
        } else if (isOperator(token)) {
            if (allowedFlag == 1) {
                allowedFlag = 0;
            } else {
                freePrec(expList);
                return SYNTACTIC_ERR;
            }
        // Left parenthese
        } else if (token->type == TOKEN_TYPE_LEFT_PARENTHESE) {
            if (allowedFlag != 0) {
                freePrec(expList);
                return SYNTACTIC_ERR;
            }
            parCount++;
        // Right parenthese
        } else if (token->type == TOKEN_TYPE_RIGHT_PARENTHESE) {
            if (allowedFlag != 1) {
                freePrec(expList);
                return SYNTACTIC_ERR;
            }
            parCount--;
        }
    
        // Modify expList, based on the type of current token
        int retVal = precTable(token, expList);

        // Expression parse tree was successfully created
        if (retVal == PRECEDENCE_END) {
            // Correct number of parentheses must be present
            if (parCount != 0) {
                freePrec(expList);
                return SYNTACTIC_ERR;    
            }
            break;
        // Error was encountered
        } else if (retVal != 0) {
            freePrec(expList);
            return retVal;
        }
    }

    // Add the parse three expression into parse tree expression root
    ptNode->content.branches.arr[0] = ((EXPNode *) expList->First->data)->content.ptNode;
    ptNode->content.branches.arrSize++;

    free(((EXPNode *) expList->First->data));
    DLDisposeSafe(expList);
    free(expList);

    return 0;
}