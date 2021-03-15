/**
 * @file controller.c
 * @author Jonáš Tichý (xtichy29@stud.fit.vutbr.cz)
 * @brief Compiler entry point with fail-safe module calls
 * @date 2020-12-03
 * 
 */
#include "controller.h"
#include "scanner.h"
#include "grammar.h"
#include "parser.h"
#include "semantics.h"
#include "generator.h"

/**
 * @brief Init nodes for syntax analysis  
 */
void initNodes() {
    If = holderIf;
    For = holderFor;
}

void ASTdispose(ASTNode *root){
    switch (root->nodeType) {
        case AST_PROG:
            if (root->content.prog.table != NULL) {
                tableDispose(root->content.prog.table);
                free(root->content.prog.table);
            }
            for(int i = 0 ; i < root->content.prog.bodyCnt;i++){
                ASTdispose(root->content.prog.body[i]);
                free(root->content.prog.body[i]);
            }
            if (root->content.prog.body != NULL) {
                free(root->content.prog.body);
            }
            free(root);
            break;
        case AST_FUNC:
            if (root->content.func.table != NULL) {
                tableDispose(root->content.func.table);
                free(root->content.func.table);
            }
            
            for(int i = 0; i < root->content.func.bodyCnt;i++){
                ASTdispose(root->content.func.body[i]);
                free(root->content.func.body[i]);
            } 
             for(int i = 0; i < root->content.func.paramsCnt;i++){
                ASTdispose(root->content.func.params[i]);
                free(root->content.func.params[i]);
            } 
            if (root->content.func.params != NULL) {
                free(root->content.func.params);
            }
            if (root->content.func.retTypes != NULL) {
                free(root->content.func.retTypes);
            }
            if (root->content.func.body != NULL) {
                free(root->content.func.body);
            }
            break; 
        case AST_RET:
            for(int i = 0; i < root->content.ret.exprCnt;i++){
                ASTdispose(root->content.ret.multiExpr[i]);
                free(root->content.ret.multiExpr[i]);
            }
            if (root->content.ret.multiExpr != NULL) {
                free(root->content.ret.multiExpr);
            }
            break;
        case AST_IF:
            if (root->content.If.table != NULL) {
                tableDispose(root->content.If.table);
                free(root->content.If.table);
            }
            if (root->content.If.elseTable != NULL) {
                tableDispose(root->content.If.elseTable);
                free(root->content.If.elseTable);
            }
            for(int i = 0 ; i < root->content.If.bodyCnt;i++){
                ASTdispose(root->content.If.body[i]);
                free(root->content.If.body[i]);
            }
            for(int i = 0; i < root->content.If.elseBodyCnt; i++){
                ASTdispose(root->content.If.elseBody[i]);
                free(root->content.If.elseBody[i]);
            }

            if (root->content.If.body != NULL) {
                free(root->content.If.body);
            }
            if (root->content.If.elseBody != NULL) {
                free(root->content.If.elseBody);
            }
            if (root->content.If.cond != NULL) {
                ASTdispose(root->content.If.cond);
                free(root->content.If.cond);
            }
            break;
        case AST_FOR:
            if(root->content.For.headerTable != NULL){
                tableDispose(root->content.For.headerTable);
                free(root->content.For.headerTable);
            }
            if(root->content.For.table != NULL){
                tableDispose(root->content.For.table);
                free(root->content.For.table);
            }
            if(root->content.For.forDec != NULL){
                ASTdispose(root->content.For.forDec);
                free(root->content.For.forDec);
            }
            if(root->content.For.forCond != NULL){
                ASTdispose(root->content.For.forCond);
                free(root->content.For.forCond);
            }
            if(root->content.For.forInc != NULL){
                ASTdispose(root->content.For.forInc);
                free(root->content.For.forInc);
            }
            for(int i = 0 ; i < root->content.For.bodyCnt;i++){
                ASTdispose(root->content.For.body[i]);
                free(root->content.For.body[i]);
            }
            if (root->content.For.body != NULL) {
                free(root->content.For.body);
            }
            break;
        case AST_DEC:
            ASTdispose(root->content.declare.expr);
            free(root->content.declare.expr);
            break;
        case AST_ASSIGN:
            for(int i = 0 ; i < root->content.assign.exprCnt;i++){
                ASTdispose(root->content.assign.multiExpr[i]);
                free(root->content.assign.multiExpr[i]);
            }
            if (root->content.assign.multiExpr != NULL) {
                free(root->content.assign.multiExpr);
            }
            if (root->content.assign.IDs != NULL) {
                free(root->content.assign.IDs);
            }
            break;
        case AST_EXPR:

            if(root->content.expr.content.binaryOp.lChild != NULL){
                ASTdispose(root->content.expr.content.binaryOp.lChild);
                free(root->content.expr.content.binaryOp.lChild);
            }
            if(root->content.expr.content.binaryOp.rChild != NULL){
                ASTdispose(root->content.expr.content.binaryOp.rChild);
                free(root->content.expr.content.binaryOp.rChild);
            }
            /*if(root->content.expr.content.unaryOp.child != NULL){
                ASTdispose(root->content.expr.content.unaryOp.child);
                free(root->content.expr.content.unaryOp.child);
            }*/
            break;
        case AST_FUNCCALL:
            if (root->content.funcCall.args != NULL) {
                free(root->content.funcCall.args);
            }
            break;
        default:
            break;
    }
}

/**
 * @brief Safely deallocates allocated memory
 * 
 * @param ptRoot Parse tree root node pointer
 * @param astRoot AST root node pointer
 * @param tokenList Token list pointer
 */
void dispose(PTNode *ptRoot, ASTNode *astRoot, tList *tokenList) {
    if (ptRoot != NULL) {
        ptNodeFree(ptRoot);
        free(ptRoot);
    }
    if (astRoot != NULL) {
        ASTdispose(astRoot);
    }
    if (tokenList != NULL) {
        DLDisposeTokenList(tokenList);
        free(tokenList);
    }
}

int main() {
    initNodes();
    setTokenSrc(stdin);   

    PTNode *ptRoot = malloc(sizeof(PTNode));
    if (ptRoot == NULL) {
        return INTERNAL_ERR;
    }
    tList *tokenList = malloc(sizeof(tList));
    if (tokenList == NULL) {
        return INTERNAL_ERR;
    }
    DLInitList(tokenList);
    int syntaxRetVal = checkSyntax(&prog, ptRoot, tokenList);
    if (syntaxRetVal != 0) {
        dispose(ptRoot, NULL, tokenList);
        return syntaxRetVal;
    }

    ASTNode *astNode = malloc(sizeof(ASTNode));
    if (astNode == NULL) {
        dispose(ptRoot, astNode, tokenList);
        return INTERNAL_ERR;
    }
    int astRetVal = buildAST(ptRoot, astNode);
    if (astRetVal != 0) {
        dispose(ptRoot, astNode, tokenList);
        return astRetVal;
    }
    
    if (astNode != NULL) {
        int retVal = checkSemantics(astNode);
        if (retVal != 0) {
            dispose(ptRoot, astNode, tokenList);
            return retVal;
        }
    }
    if (astNode != NULL) {
        generatorInit(astNode);
    }

    dispose(ptRoot, astNode, tokenList);
    return 0;
}