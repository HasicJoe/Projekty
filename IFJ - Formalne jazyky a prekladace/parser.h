/**
 * @file parser.h
 * @author Jonáš Tichý (xtichy29@stud.fit.vutbr.cz)
 * @author Miroslav Štěpánek (xstepa68@stud.fit.vutbr.cz)
 * @brief Parser providing syntax and AST construction functions
 * @date 2020-10-22
 */
#ifndef PARSER
#define PARSER
#include "scanner.h"
#include "symtable.h"
#include "list.h"

#define MAX_OPTS 15
#define MAX_OPT_SIZE 15
#define REDUCTION_MARKER 50
#define PRECEDENCE_END 100

typedef enum {NODE, LEAF}GrammarNodeType;

// Parse tree node types
typedef enum {
    PT_LEAF,

    // prog
    PT_PROG,
    PT_PROG_BODY,
    PT_PROG_PACKAGE,
    
    // func
    PT_FUNC,
    PT_FUNC_RET_TYPES,
    PT_FUNC_PARAMS,
    PT_RET_TYPES,
    PT_PARAMS,

    //for
    PT_FOR,
    PT_FOR_INC,
    PT_FOR_COND,
    PT_FOR_DEC,

    // if + body
    PT_IF,
    PT_BODY,

    // statements
    PT_STATEMENT,
    PT_ASSIGN,
    PT_RET,
    PT_DEFINE,
    PT_MULTIEXPR,
    PT_EXPR,
    PT_FUNC_CALL,
    PT_FUNC_CALL_ARGS,

    // expr
    PT_MULTI_ID,
    PT_OPERAND,
    PT_OPERATOR,
    PT_TYPE,
    PT_R_PARENTH_OPT,
    PT_L_PARENTH_OPT,
    PT_MULTI_EOL,
    PT_EOL_OPT,
    PT_EPS
}ParseType;


// Parse tree node struct
typedef struct ptNode{
    GrammarNodeType type;
    ParseType parseType;
    union {
        Token *token;
        struct {
            struct ptNode *arr[MAX_OPT_SIZE];
            int arrSize;
        }branches;
    }content;
}PTNode;

// Grammar node struct
typedef struct mNode {
    GrammarNodeType type;
    ParseType parseType;
    union {
        TokenType TokenType;
        struct {
            struct mNode *opts[MAX_OPTS][MAX_OPT_SIZE];
            int optCnt;
            int optSize[MAX_OPTS];
        }branches;
    }content;
}GrammarNode;


// AST
typedef enum {
    AST_PROG,
    AST_FUNC,
    AST_FUNC_PARAM,
    AST_IF,
    AST_FOR,
    AST_DEC,
    AST_ASSIGN,
    AST_FUNCCALL,
    AST_RET,
    AST_EXPR,
    AST_LIT
}NodeTypeAST;


struct ASTProg {
    struct astNode **body;
    int bodyCnt;
    TablePtr table;
};

struct ASTFunc {
    Token *ID;
    struct astNode **params;
    int paramsCnt; 
    Token **retTypes;
    int retTypesCnt;
    struct astNode **body;
    int bodyCnt;
    TablePtr table;
};

struct ASTFuncParam {
    Token *ID;
    Token *type;
};

struct ASTIf {
    struct astNode *cond;
    struct astNode **body;
    int bodyCnt;
    struct astNode **elseBody;
    int elseBodyCnt;
    TablePtr table;
    TablePtr elseTable;
};

struct ASTFor {
    struct astNode *forDec;
    struct astNode *forCond;
    struct astNode *forInc;
    struct astNode **body;
    int bodyCnt;
    TablePtr table;
    TablePtr headerTable;
};

struct ASTAssign {
    Token **IDs;
    int IDCnt;
    struct astNode **multiExpr;
    int exprCnt;
};

struct ASTDeclare {
    Token *ID;
    struct astNode *expr;
};

typedef enum {
    EXPR_ADD,
    EXPR_SUB,
    EXPR_MUL,
    EXPR_DIV,
    EXPR_GT,
    EXPR_GTE,
    EXPR_LT,
    EXPR_LTE,
    EXPR_EQ,
    EXPR_NEQ
}OperatorType;

struct ASTExpr {
    OperatorType exprType;
    union {
        struct {
            struct astNode *lChild;
            struct astNode *rChild;
        }binaryOp;
        struct {
            struct astNode *child;
        }unaryOp;
    }content;
};

struct ASTLit {
    Token *token;
};

struct ASTFuncCall {
    Token *ID;
    Token **args;
    int argsCnt;
};

struct ASTRet {
    struct astNode **multiExpr;
    int exprCnt;
};

typedef struct astNode {
    NodeTypeAST nodeType;
    union {
        struct ASTProg prog;
        struct ASTFunc func;
        struct ASTFuncParam funcParam;
        struct ASTFuncCall funcCall;
        struct ASTDeclare declare;
        struct ASTAssign assign;
        struct ASTRet ret;
        struct ASTExpr expr;
        struct ASTIf If;
        struct ASTFor For;
        struct ASTLit lit;
    }content;
}ASTNode;
// End of AST

// Expression struct used in precedence
typedef struct expNode {
    GrammarNodeType itemType;
    struct tDLExpElem *rptr;
    union {
        Token *token;
        PTNode *ptNode;
    }content;
} EXPNode;

int checkSyntax(GrammarNode *, PTNode *, tList *);
int buildAST(PTNode *, ASTNode *);
int progNode(PTNode *, ASTNode *);
int funcNode(PTNode *, ASTNode *);
int forNode(PTNode *, ASTNode *);
int ifNode(PTNode *, ASTNode *);
int declareNode(PTNode *, ASTNode *);
int assignNode(PTNode *, ASTNode *);
int retNode(PTNode *, ASTNode *);
int funcCallNode(PTNode *, ASTNode *);
int exprNode(PTNode *, ASTNode *);
int getOpPrio(Token *);
int precExpr(PTNode* , tList *);
int PTtoAST(PTNode *, ASTNode *);
bool isOperator(Token *);
bool isLitOrID(Token *);
void ptNodeFree(PTNode*);
void printExp(ASTNode *, int);
void tokenFromPTNode(PTNode *, tList *);

#endif