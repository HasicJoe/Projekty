/**
 * @file generator.c
 * @author Samuel Valaštín (xvalas10@stud.fit.vutbr.cz)
 * @author Daniel Gavenda (xgaven08@stud.fit.vutbr.cz)
 * @brief Generator of three address code
 * @date 2020-11-18
 * 
 */

#include "generator.h"

/// helper global vars
int progCounter = 0;
int idCounter = 0;      
int paramCounter = 0;
int returnCounter = 0;
int printTermCounter = 0;
int concatCounter = 0;
int printParamsCount[MAX_SCOPE];
int ifDefCount = 0;
int printCounter = 0;
int ifCounter = 0;
int tmpDef = 0;
int tmpCounter = 1;
int forScope = 0;
int forBodyScope = 0;
int mainRet = 0;

/// built-in functions
int bInputs = 0;
int bInputf = 0;
int bInputi = 0;
int bPrint = 0;
int bInt2float = 0;
int bFloat2int = 0;
int bLen = 0;
int bSubstr = 0;
int bOrd = 0;
int bChr = 0;
/// built-in function names
char* funcNames[10]= {"inputs","inputi","inputf","print","int2float","float2int","len","substr","ord","chr"};

///list that includes all DEFVAR
tList *defList = NULL;

/** @brief Function generate HEADER of each function
*/
void funcGenerate(char *funcName, bool isMain){
    LABEL(funcName);
    if(isMain) {
        CREATEFRAME
    }
    PUSHFRAME
}

/**
 * @brief generate built-in function inputs() (string, int)
 * 
 */
void inputsGenerate(){
    funcGenerate(funcNames[INPUTS],false);
    fprintf(stdout,"DEFVAR LF@returnValue0\n");
    fprintf(stdout,"MOVE LF@returnValue0 string@nil\n");
    fprintf(stdout,"DEFVAR LF@returnValue1\n");
    fprintf(stdout,"MOVE LF@returnValue1 int@0\n");
    //          func body           //
    fprintf(stdout, "READ LF@returnValue0 string\n");
    fprintf(stdout, "JUMPIFNEQ inputsnoerror LF@returnValue0 nil@nil\n");
    fprintf(stdout, "MOVE LF@returnValue1 int@1\n");
    fprintf(stdout, "LABEL inputsnoerror\n");
    FUNCEND
}

/**
 * @brief generate built-in function inputi() (int, int)
 * 
 */
void inputiGenerate(){
    funcGenerate(funcNames[INPUTI],false);
    fprintf(stdout,"DEFVAR LF@returnValue0\n");
    fprintf(stdout,"MOVE LF@returnValue0 nil@nil\n");
    fprintf(stdout,"DEFVAR LF@returnValue1\n");
    fprintf(stdout,"MOVE LF@returnValue1 int@0\n");
    //          func body           //
    fprintf(stdout, "READ LF@returnValue0 int\n");
    fprintf(stdout, "JUMPIFNEQ inputinoerror LF@returnValue0 nil@nil\n");
    fprintf(stdout, "MOVE LF@returnValue1 int@1\n");
    fprintf(stdout, "LABEL inputinoerror\n");
    FUNCEND
}

/**
 * @brief generate built-in unction inputf() (float, int)
 * 
 */
void inputfGenerate(){
    funcGenerate(funcNames[INPUTF],false);
    fprintf(stdout,"DEFVAR LF@returnValue0\n");
    fprintf(stdout,"MOVE LF@returnValue0 float@%a\n", 0.0);
    fprintf(stdout,"DEFVAR LF@returnValue1\n");
    fprintf(stdout,"MOVE LF@returnValue1 int@0\n");
    //          func body           //
    fprintf(stdout, "READ LF@returnValue0 float\n");
    fprintf(stdout, "JUMPIFNEQ inputfnoerror LF@returnValue0 nil@nil\n");
    fprintf(stdout, "MOVE LF@returnValue1 int@1\n");
    fprintf(stdout, "LABEL inputfnoerror\n");
    FUNCEND
}

/**
 * @brief generate built-in function int2float(int) (float)
 * 
 */
void int2floatGenerate(){
    funcGenerate(funcNames[INTTOFLOAT],false);
    fprintf(stdout,"DEFVAR LF@returnValue0\n");
    fprintf(stdout,"MOVE LF@returnValue0 nil@nil\n");
    //          func body           //
    fprintf(stdout,"PUSHS LF@arg0\n");
    fprintf(stdout,"INT2FLOATS\n");
    fprintf(stdout,"POPS LF@returnValue0\n");
    //          func body           //
    FUNCEND
}

/**
 * @brief generate built-in function float2int(float) (int)
 * 
 */
void float2intGenerate(){
    funcGenerate(funcNames[FLOATTOINT],false);
    fprintf(stdout,"DEFVAR LF@returnValue0\n");
    fprintf(stdout,"MOVE LF@returnValue0 nil@nil\n");
    //          func body           //
    fprintf(stdout,"PUSHS LF@arg0\n");
    fprintf(stdout,"FLOAT2INTS\n");
    fprintf(stdout,"POPS LF@returnValue0\n");
    //          func body           //
    FUNCEND
}

/**
 * @brief generate built-in function len(string) (int)
 * 
 */
void lenGenerate(){
    funcGenerate(funcNames[LEN],false);
    fprintf(stdout,"DEFVAR LF@returnValue0\n");
    fprintf(stdout,"MOVE LF@returnValue0 int@0\n");
    //          func body           //
    fprintf(stdout,"STRLEN LF@returnValue0 LF@arg0\n");
     //          func body           //
    FUNCEND
}

/**
 * @brief Generate built-in function substr(string, int, int) (string, int)
 * 
 */
void substrGenerate(){
    funcGenerate(funcNames[SUBSTR],false);
    fprintf(stdout,"DEFVAR LF@returnValue0\n");
    fprintf(stdout,"MOVE LF@returnValue0 string@\n");
    fprintf(stdout,"DEFVAR LF@returnValue1\n");
    fprintf(stdout,"MOVE LF@returnValue1 int@0\n");
    //          func body           //
    // bounds checking
    /// checking bounds (i >= 0)
    fprintf(stdout, "PUSHS LF@arg1\n");
    fprintf(stdout, "PUSHS int@0\n");
    fprintf(stdout, "LTS\n");
    fprintf(stdout, "PUSHS bool@true\n");
    fprintf(stdout, "JUMPIFEQS substrerror\n");
    ///checking bounds (i < strlen)
    fprintf(stdout, "DEFVAR LF@length\n");
    fprintf(stdout, "STRLEN LF@length LF@arg0\n");
    fprintf(stdout, "PUSHS LF@arg1\n");
    fprintf(stdout, "PUSHS LF@length\n");
    fprintf(stdout, "LTS\n");
    fprintf(stdout, "PUSHS bool@false\n");
    fprintf(stdout, "JUMPIFEQS substrerror\n");
    ///checking bounds (n >= 0)
    fprintf(stdout, "PUSHS LF@arg2\n");
    fprintf(stdout, "PUSHS int@0\n");
    fprintf(stdout, "LTS\n");
    fprintf(stdout, "PUSHS bool@true\n");
    fprintf(stdout, "JUMPIFEQS substrerror\n");
    ///init cycle
    fprintf(stdout, "DEFVAR LF@curr\n");
    fprintf(stdout, "MOVE LF@curr string@\n");
    /// main cycle
    fprintf(stdout, "LABEL substrcycle\n");
    fprintf(stdout, "PUSHS LF@arg2\n");
    fprintf(stdout, "PUSHS int@0\n");
    fprintf(stdout, "GTS\n");
    fprintf(stdout, "PUSHS bool@true\n");
    fprintf(stdout, "JUMPIFNEQS substrreturn\n");
    fprintf(stdout, "PUSHS LF@arg1\n");
    fprintf(stdout, "PUSHS LF@length\n");
    fprintf(stdout, "LTS\n");
    fprintf(stdout, "PUSHS bool@false\n");
    fprintf(stdout, "JUMPIFEQS substrreturn\n");
    /// cycle body concatening the individual chars in substr range
    fprintf(stdout, "GETCHAR LF@curr LF@arg0 LF@arg1\n");
    fprintf(stdout, "CONCAT LF@returnValue0 LF@returnValue0 LF@curr\n");
    fprintf(stdout, "ADD LF@arg1 LF@arg1 int@1\n");
    fprintf(stdout, "SUB LF@arg2 LF@arg2 int@1\n");
    fprintf(stdout, "JUMP substrcycle\n");
    ///return
    fprintf(stdout, "LABEL substrreturn\n");
    fprintf(stdout, "POPFRAME\n");
    fprintf(stdout, "RETURN\n");
    ///error return
    fprintf(stdout, "LABEL substrerror\n");
    fprintf(stdout, "MOVE LF@returnValue1 int@1\n");
    FUNCEND
}

/**
 * @brief generate built-in function print(...)
 * 
 * @param termCounter number of parameters
 * @param identifier id of a print function (needed when generates with multiple arg counts)
 */
void printGenerate(int termCounter,int identifier){

    fprintf(stdout,"LABEL %s%d\n",funcNames[PRINT],identifier);
    fprintf(stdout,"PUSHFRAME\n");
    for(int i = 0 ; i < termCounter;i++){
        fprintf(stdout,"WRITE LF@arg%d\n",i);
    }
    FUNCEND
}

/**
 * @brief generate built-in function ord(string, int) (int, int)
 * 
 */
void ordGenerate(){
     funcGenerate(funcNames[ORD],false);
    fprintf(stdout,"DEFVAR LF@returnValue0\n");
    fprintf(stdout,"MOVE LF@returnValue0 string@\n");
    fprintf(stdout,"DEFVAR LF@returnValue1\n");
    fprintf(stdout,"MOVE LF@returnValue1 int@0\n");
    //          func body           //
    fprintf(stdout,"DEFVAR LF@correctIndex\n");
    // compare index with zero
    fprintf(stdout,"PUSHS LF@arg1\n");
    fprintf(stdout,"PUSHS int@0\n");
    fprintf(stdout,"LTS\n");
    fprintf(stdout,"POPS LF@correctIndex\n");
    fprintf(stdout,"JUMPIFEQ ordErrLabel LF@correctIndex bool@true\n");
    fprintf(stdout,"CLEARS\n");
    // compare with stringlen - 1
    fprintf(stdout,"DEFVAR LF@stringLen\n");
    fprintf(stdout,"STRLEN LF@stringLen LF@arg0\n");
    fprintf(stdout,"PUSHS LF@stringLen\n");
    fprintf(stdout,"PUSHS int@1\n"); // len(s)-1
    fprintf(stdout,"SUBS\n");
    fprintf(stdout,"PUSHS LF@arg1\n"); // i
    fprintf(stdout,"LTS\n");
    fprintf(stdout,"POPS LF@correctIndex\n");
    fprintf(stdout,"JUMPIFEQ ordErrLabel LF@correctIndex bool@true\n");
    // save correct result
    fprintf(stdout,"STRI2INT LF@returnValue0 LF@arg0 LF@arg1\n");
    fprintf(stdout,"JUMP ordEndLabel\n");
    // err label
    fprintf(stdout,"LABEL ordErrLabel\n");
    fprintf(stdout,"MOVE LF@returnValue1 int@1\n");
    fprintf(stdout,"LABEL ordEndLabel\n");
    //          func body           //
    FUNCEND
}

/**
 * @brief generate built-in function ord(string, int) (int, int)
 * 
 */
void chrGenerate(){
    funcGenerate(funcNames[CHR],false);
    fprintf(stdout,"DEFVAR LF@returnValue0\n");
    fprintf(stdout,"MOVE LF@returnValue0 int@0\n");
    fprintf(stdout,"DEFVAR LF@returnValue1\n");
    fprintf(stdout,"MOVE LF@returnValue1 int@0\n");
    //          func body           //
    fprintf(stdout,"DEFVAR LF@condResult\n");
    // compare with zero
    fprintf(stdout,"PUSHS LF@arg0\n");
    fprintf(stdout,"PUSHS int@0\n");
    fprintf(stdout,"LTS\n");
    fprintf(stdout,"POPS LF@condResult\n");
    fprintf(stdout,"JUMPIFEQ chrErrLabel LF@condResult bool@true\n");
    fprintf(stdout,"CLEARS\n");
    // compare with 255
    fprintf(stdout,"PUSHS LF@arg0\n");
    fprintf(stdout,"PUSHS int@255\n");
    fprintf(stdout,"GTS\n");
    fprintf(stdout,"POPS LF@condResult\n");
    fprintf(stdout,"JUMPIFEQ chrErrLabel LF@condResult bool@true\n");
    fprintf(stdout,"CLEARS\n");
    // return chr
    fprintf(stdout, "INT2CHAR LF@returnValue0 LF@arg0\n");
    fprintf(stdout, "JUMP chrEndLabel\n");
    // return error value
    fprintf(stdout,"LABEL chrErrLabel\n");
    fprintf(stdout,"MOVE LF@returnValue1 int@1\n");
    fprintf(stdout,"JUMP chrEndLabel\n");
    fprintf(stdout,"LABEL chrEndLabel\n");
    //          func body           //
    FUNCEND
}

/**
 * @brief generate token dependant on its type and its scope
 * @param token 
 * @param symTables 
 */
void tokenGenerate(Token *token, tList *symTables){
    if(token->type == TOKEN_TYPE_IDENTIFIER){
        int scope = 0;
        findSymbol(token->attribute.string, symTables, &scope);
        fprintf(stdout, "LF@%s*%d ", token->attribute.string, SCOPE-scope);
    } else if(token->type == TOKEN_TYPE_STRING){
        fprintf(stdout,"string@");
        ///converting string to ifj20code notation
        for (int i = 0; token->attribute.string[i] != '\0'; i++) {
            unsigned char ch = (unsigned) token->attribute.string[i];
            if (ch < 10) {
                fprintf(stdout, "\\00%d",token->attribute.string[i]);
            } else if (ch < 33 || ch == 35 || ch == 92) {
                fprintf(stdout,"\\0%d", ch);
            } else {
                fprintf(stdout,"%c",ch);
            }
        }
    } else if(token->type == TOKEN_TYPE_DOUBLE){
        fprintf(stdout,"float@%a ",token->attribute.float64); 
    } else if(token->type == TOKEN_TYPE_INTEGER){
        fprintf(stdout,"int@%ld ",token->attribute.integer); 
    }
}
/**
 * @brief does the same as generateToken() but checks only symbols that are initialized
 * @param token 
 * @param symTables 
 */
void tokenGenerateToAssign(Token *token, tList *symTables){
    if(token->type == TOKEN_TYPE_IDENTIFIER){
        int scope = getSymbolScope(token->attribute.string, symTables);
        fprintf(stdout, "LF@%s*%d ", token->attribute.string, SCOPE-scope);
    } else if(token->type == TOKEN_TYPE_STRING){
        fprintf(stdout,"string@");
        ///converting string
        for (int i = 0; token->attribute.string[i] != '\0'; i++) {
            unsigned char ch = (unsigned) token->attribute.string[i];
            if (ch < 10) {
                fprintf(stdout, "\\00%d",token->attribute.string[i]);
            } else if (ch < 33 || ch == 35 || ch == 92) {
                fprintf(stdout,"\\0%d", ch);
            } else {
                fprintf(stdout,"%c",ch);
            }
        }
    } else if(token->type == TOKEN_TYPE_DOUBLE){
        fprintf(stdout,"float@%a ",token->attribute.float64); 
        //pozriet ako vypisovat floaty v zadani
    } else if(token->type == TOKEN_TYPE_INTEGER){
        fprintf(stdout,"int@%ld ",token->attribute.integer); 
    }
}

/**
 * @brief Generate concatenation between two string literals or str identif
 */
void concatGenerate(){
    DEFFREEVAR
    fprintf(stdout,"LF@concatVariable%d",++concatCounter);
    NEWLINE
    DEFFREEVAR
    fprintf(stdout,"LF@concatLChild%d",concatCounter);
    NEWLINE
    DEFFREEVAR
    fprintf(stdout,"LF@concatRChild%d",concatCounter);
    NEWLINE
    POPSFREE
    fprintf(stdout,"LF@concatLChild%d",concatCounter);
    NEWLINE
    POPSFREE
    fprintf(stdout,"LF@concatRChild%d",concatCounter);
    NEWLINE
    fprintf(stdout,"CONCAT LF@concatVariable%d LF@concatRChild%d LF@concatLChild%d\n",concatCounter,concatCounter,concatCounter);
    fprintf(stdout,"PUSHS LF@concatVariable%d\n",concatCounter);
}

/**
 * @brief Generate if (stack usage)
 * @param root AST subtree node
 * @param symTables 
 */
void ifGenerate(ASTNode *root, tList *symTables){
    static int ifId = 0;
    int id = ifId++; 
    ///generate condition and jump
    exprGenerate(root->content.If.cond,symTables);
    fprintf(stdout, "PUSHS bool@true\n");
    fprintf(stdout, "JUMPIFEQS ifBody%d\n",id);
    /*          ELSE BODY                */
    ///push elsebody symtable into the list, if exists
    if (root->content.If.elseBodyCnt > 0) {
        DLInsertFirst(symTables, (TablePtr) root->content.If.elseTable);
    }
    for(int i = 0 ; i < root->content.If.elseBodyCnt;i++){
        codeGenerate(root->content.If.elseBody[i], symTables);
    }
    //pop elsebody symtable, if exists
    if (root->content.If.elseBodyCnt > 0) {
        DLDeleteFirst(symTables);
    }

    fprintf(stdout,"JUMP endIfLabel%d\n",id); // jump after else
    /*                IF BODY                      */
    fprintf(stdout,"LABEL ifBody%d\n",id);
    ///push ifbody symtable into the list
    DLInsertFirst(symTables, (TablePtr) root->content.If.table);
    for(int i = 0 ; i < root->content.If.bodyCnt;i++){
        codeGenerate(root->content.If.body[i], symTables);
    }
    ///pop ifbody table
    DLDeleteFirst(symTables);
    fprintf(stdout,"LABEL endIfLabel%d\n",id); // end of condition label
}

/**
 * @brief Generate expressions
 * @param root AST subtree node
 * @param symTables 
 */
void exprGenerate(ASTNode *root, tList *symTables) {
    ASTNode* tmp = root;
    bool floatDiv = false;
    bool toConc = false; 
    OperatorType exprType = root->content.expr.exprType;

    if (exprType == EXPR_GTE) {
        root->content.expr.exprType = EXPR_GT;
        exprGenerate(root, symTables);
        root->content.expr.exprType = EXPR_EQ;
        exprGenerate(root, symTables);
        ORS
    } else if (exprType == EXPR_LTE) {
        root->content.expr.exprType = EXPR_LT;
        exprGenerate(root, symTables);
        root->content.expr.exprType = EXPR_EQ;
        exprGenerate(root, symTables);
        ORS
    } else {
        ///Determining types of identifiers or literals (needed only when adding or dividing)
        if (exprType == EXPR_ADD || exprType == EXPR_DIV) {
            while (tmp->nodeType == AST_EXPR) {
                tmp = tmp->content.expr.content.binaryOp.lChild;
            }
            Token *t = tmp->content.lit.token;
            if (t->type == TOKEN_TYPE_STRING) {
                toConc = true;
                floatDiv = false;
            } else if (t->type == TOKEN_TYPE_INTEGER) {
                toConc = false;
                floatDiv = false;
            } else if (t->type == TOKEN_TYPE_DOUBLE) {
                toConc = false;
                floatDiv = true;
            } else {
                Item *it = findSymbol(t->attribute.string, symTables, NULL);
                if (it->content.var.dataType == TOKEN_TYPE_STRING) {
                    toConc = true;
                    floatDiv = false;
                } else if (it->content.var.dataType == TOKEN_TYPE_DOUBLE) {
                    toConc = false;
                    floatDiv = true;
                } else {
                    toConc = false;
                    floatDiv = false;
                }
            }
        }

        ///recursively going through expression tree
        ///left child nesting
        if (root->content.expr.content.binaryOp.lChild->nodeType == AST_EXPR) {
            exprGenerate(root->content.expr.content.binaryOp.lChild, symTables);
        } else {
            PUSHSFREE
            tokenGenerateToAssign(root->content.expr.content.binaryOp.lChild->content.lit.token, symTables);
            NEWLINE
        }
        ///right child nesting
        if (root->content.expr.content.binaryOp.rChild->nodeType == AST_EXPR) {
            exprGenerate(root->content.expr.content.binaryOp.rChild, symTables);
        } else {
            PUSHSFREE
            tokenGenerateToAssign(root->content.expr.content.binaryOp.rChild->content.lit.token, symTables);
            NEWLINE
        }
        ///generating if20code expression
        if (exprType == EXPR_SUB) {
            SUBS
        } else if (exprType == EXPR_MUL) {
            MULS
        } else if (exprType == EXPR_ADD) {
            if (toConc) {
                concatGenerate();
            } else {
                ADDS
            }
        } else if (exprType == EXPR_DIV) {
            if (floatDiv) {
                DIVS
            } else {
                IDIVS
            }               
        } else if (exprType == EXPR_GT) {
            GTS
        } else if (exprType == EXPR_LT) {
            LTS
        } else if (exprType == EXPR_EQ) {
            EQS
        } else if (exprType == EXPR_NEQ) {
            EQS
            NOTS
        }
    }
}

/**
 * @brief generate declarations as assigns inside for cycle
 * @param root AST subtree node
 * @param symTables 
 */
void forAssGen(ASTNode *root,tList *symTables){
    if(root->content.declare.expr->nodeType == AST_LIT) {
        MOVEFREE
        tokenGenerate(root->content.declare.ID, symTables);
        tokenGenerateToAssign(root->content.declare.expr->content.lit.token, symTables);
        NEWLINE
        symbolInit(root->content.declare.ID, symTables);
    } else if(root->content.declare.expr->nodeType == AST_EXPR) {
        exprGenerate(root->content.declare.expr, symTables);
        POPSFREE
        tokenGenerate(root->content.declare.ID, symTables);
        NEWLINE
        symbolInit(root->content.declare.ID, symTables);
    }   
}

/**
 * @brief pull declarations from inside for loops
 * @param root 
 * @param symTables 
 */
void pullDeclarations(ASTNode *root, tList *symTables) {

    ///creating temporary frames by adding symtables
    /// pushing header table
    DLInsertFirst(symTables, (TablePtr) root->content.For.headerTable);
    if (root->content.For.forDec != NULL) {
        if (!isSymbDef(root->content.For.forDec->content.declare.ID, symTables)) {
            DEFFREEVAR
            tokenGenerate(root->content.For.forDec->content.declare.ID, symTables);
            NEWLINE
            int scope = 0;
            findSymbol(root->content.For.forDec->content.declare.ID->attribute.string, symTables, &scope);
            pushSymbol((SCOPE-scope), root->content.For.forDec->content.declare.ID->attribute.string);
        }
    }
    /// pushing body table
    DLInsertFirst(symTables, (TablePtr) root->content.For.table);
    for(int i = 0; i < root->content.For.bodyCnt; i++) {
        if (root->content.For.body[i]->nodeType == AST_DEC) {
            if (!isSymbDef(root->content.For.body[i]->content.declare.ID, symTables)) {
                DEFFREEVAR
                tokenGenerate(root->content.For.body[i]->content.declare.ID, symTables);
                NEWLINE
                int scope = 0;
                findSymbol(root->content.For.body[i]->content.declare.ID->attribute.string, symTables, &scope);
                pushSymbol((SCOPE-scope), root->content.For.body[i]->content.declare.ID->attribute.string);

            }
        }
        if (root->content.For.body[i]->nodeType == AST_FOR) {
            pullDeclarations(root->content.For.body[i], symTables);
        }
    }
    ///popping both tables (header & body)
    DLDeleteFirst(symTables);
    DLDeleteFirst(symTables);
}

/**
 * @brief generate for cycles
 * @param root current node
 * @param symTables list of all symtables
 */
void forGenerate(ASTNode *root, tList *symTables) {
    static int forId = 0;
    int id = ++forId;
        pullDeclarations(root, symTables);    
    ///push forheader symTable (important to push after decl pull)
    DLInsertFirst(symTables, (TablePtr) root->content.For.headerTable);
    ///for declaration (is exists)
    if (root->content.For.forDec != NULL) {
        if(root->content.For.forDec->nodeType == AST_DEC) {
            forAssGen(root->content.For.forDec, symTables);
        }
    }
    fprintf(stdout,"LABEL FOR%d\n",id);
    ///for condition
    if(root->content.For.forCond->nodeType == AST_EXPR){
        exprGenerate(root->content.For.forCond, symTables);
        PUSHS("bool@true");
        fprintf(stdout,"JUMPIFNEQS ForEnd%d\n",id);
    }

    ///push forbody symtable
    DLInsertFirst(symTables, (TablePtr) root->content.For.table);
    ///for body
    for(int i = 0 ; i < root->content.For.bodyCnt; i++){
        if(root->content.For.body[i]->nodeType == AST_FOR){
        }
        if(root->content.For.body[i]->nodeType == AST_DEC) {
            forAssGen(root->content.For.body[i], symTables);             
        } else {
            codeGenerate(root->content.For.body[i], symTables);
        }
    }
    ///pop forbody symtable
    DLDeleteFirst(symTables);
    ///generate increment (out of body scope)
    codeGenerate(root->content.For.forInc,symTables);
    /// jumping after body is popped so we have the same scope (hopefully)
    fprintf(stdout,"JUMP FOR%d\n",id);
    fprintf(stdout,"LABEL ForEnd%d\n", id);
    ///pop forheader symtable
    DLDeleteFirst(symTables);
}

/**
 * @brief determine what built-in functions to generate
 */
void buletInGenerate(){

    if(bInputs){
        inputsGenerate();
    }if(bInputf){
        inputfGenerate();
    }if(bInputi){
        inputiGenerate();
    } if(bPrint){
        for(int i = 0 ; i < printCounter;i++){
            printGenerate(printParamsCount[i],i+1);
        }
    }if(bInt2float){
        int2floatGenerate();
    }if(bFloat2int){
        float2intGenerate();
    }if(bOrd){
        ordGenerate();
    }if(bSubstr){
        substrGenerate();
    }if(bLen){
        lenGenerate();
    }if(bChr){
        chrGenerate();
    }
}

/**
 * @brief find symbol in the nearest frame
 * @param id identifier name
 * @param symTables list of symtables 
 * @param scope returns a curent scope
 * @return Item* found item in the nearest table
 */
Item *findSymbol(char *id, tList *symTables, int *scope) {
    tDLElemPtr elemPtr = symTables->First;
    Item *res = NULL;

    while(elemPtr != NULL) {
        res = tableLookup(id,(TablePtr) elemPtr->data);
        if (res != NULL) {
            return res;
        }
        elemPtr = elemPtr->rptr;
        ///in case of finding symbol without needing its scope
        if (scope != NULL) {
            (*scope)++;
        }
    }
    return res;
}
/**
 * @brief get the first init. symbol
 * @param id 
 * @param symTables 
 * @return int scope of the first initialized symbol
 */
int getSymbolScope(char *id, tList *symTables) {
    tDLElemPtr symPtr = symTables->First;
    tDLElemPtr defPtr = NULL;
    int scope = 0;
    Item *item = NULL;
    Symb *symb = NULL;
    while(symPtr != NULL) {
        item = tableLookup(id, (TablePtr) symPtr->data);

        if (item != NULL) {
            defPtr = defList->First;
            while(defPtr != NULL) {
                symb = (Symb*) defPtr->data;
                if (!strcmp(id, symb->name) && symb->scope == (SCOPE-scope) && symb->init) {
                    return scope;
                }
                defPtr = defPtr->rptr;
            }
        }
        scope++;
        symPtr = symPtr->rptr;
    }
    return 0;
}

/**
 * @brief find if symbol with that name and scope was already DEFVARed in function
 * @param token symbol 
 * @param symTables symTable
 * @return true is defined
 * @return false is not defined
 */
bool isSymbDef(Token *token, tList *symTables) {
    tDLElemPtr item = defList->First;
    int scope = 0;
    findSymbol(token->attribute.string, symTables, &scope);
    while (item != NULL) {
        Symb *symb = (Symb*) item->data;
        if ((SCOPE-scope) == symb->scope && !strcmp(token->attribute.string, symb->name)) {
                return true;
        }
        item = item->rptr;
    }
    return false;
}

/**
 * @brief add a symbol to deflist
 * @param scope 
 * @param name 
 */
void pushSymbol(int scope, char *name) {
    Symb *s = (Symb*) malloc(sizeof(Symb));
    if (s == NULL) {
        return;
    }
    s->name = (char*) malloc(strlen(name)+1);
    if (s->name == NULL) {
        return;
    }
    strcpy(s->name, name);
    s->scope = scope;
    s->init = false;
    DLInsertFirst(defList, (Symb*) s);
}

/**
 * @brief inform defineList that symbol was initialized
 * @param name of the symbol  
 * @param symTables 
*/
void symbolInit(Token *token, tList *symTables) {
    int scope = 0;
    findSymbol(token->attribute.string, symTables, &scope);
    tDLElemPtr item = defList->First;
    while (item != NULL) {
        Symb *s = (Symb*) item->data;
        if (!strcmp(token->attribute.string, s->name) && (SCOPE-scope) == s->scope) {
            s->init = true;
            return;
        }
        item = item->rptr;
    }
}
/* @brief Clears symbols from symtable
*/
void clearSymbols() {
    tDLElemPtr item = defList->First;
    while (item != NULL) {
        Symb* symb = (Symb*) item->data;
        free(symb->name);
        free(item->data);
        item = item->rptr;
    }
}

/**
 * @brief function to start code generation
 * @param root start of the AST
 * @return int status code 
 */
int generatorInit(ASTNode *root) {
    ///root check
    if (root == NULL) {
        return SEMANTIC_OTHER_ERR;
    }
    ///symtable list init
    tList* symTables = (tList*) malloc(sizeof(tList));
    if (symTables == NULL) {
        return INTERNAL_ERR;
    }
    DLInitList(symTables);
    
    defList = (tList*) malloc(sizeof(tList));
    if (defList == NULL) {
        return INTERNAL_ERR;
    }
    DLInitList(defList);

    ///generate asm header
    GENERATE_HEADER
    for (int i = 0; i < root->content.prog.bodyCnt; i++) {
        codeGenerate(root->content.prog.body[i], symTables);
    }
    ///generate built-in functions
    buletInGenerate();

    ///empty the symtable list
    /// individual symtables has to be cleared separately
    free(symTables);

    DLDisposeSafe(defList);
    free(defList);
    return SUCCESS;
}

/**
 * @brief Generate all if conditional vars inside for
 * @param root AST subtree node
 */
void defvarsInsideFor(ASTNode * root){
    for(int i = 0; i < root->content.For.bodyCnt;i++){
        if(root->content.For.body[i]->nodeType == AST_FOR){
            defvarsInsideFor(root->content.For.body[i]);
        }
        if(root->content.For.body[i]->nodeType == AST_ASSIGN){
            if(root->content.For.body[i]->content.assign.exprCnt > 1){
                for(int j = 0 ; j < root->content.For.body[i]->content.assign.exprCnt;j++){
                    if(strcmp(root->content.For.body[i]->content.assign.IDs[j]->attribute.string,"_") != 0){
                        DEFFREEVAR
                        tmpDef++;
                        TMPVAR(tmpDef);
                        NEWLINE
                    }
                }
            }   
        }
    }
}

/**
 * @brief Generate all if conditional inside if body
 * @param root AST subtree node
 */
void defVarsInsideIf(ASTNode *root){

     for(int i = 0; i < root->content.If.bodyCnt;i++){
         if(root->content.If.body[i]->nodeType == AST_FOR){
            defvarsInsideFor(root->content.If.body[i]);
         }
        if(root->content.If.body[i]->nodeType == AST_ASSIGN){
            if(root->content.If.body[i]->content.assign.exprCnt > 1){
                for(int j = 0 ; j < root->content.If.body[i]->content.assign.exprCnt;j++){
                    if(strcmp(root->content.If.body[i]->content.assign.IDs[j]->attribute.string,"_") != 0){
                        DEFFREEVAR
                        tmpDef++;
                        TMPVAR(tmpDef);
                        NEWLINE
                    }
                }
            }   
        }
    }
    for(int j = 0 ; j < root->content.If.elseBodyCnt;j++){
        if(root->content.If.elseBody[j]->nodeType == AST_IF){
            defVarsInsideIf(root->content.If.elseBody[j]);
        }
        if(root->content.If.elseBody[j]->nodeType == AST_FOR){
            defvarsInsideFor(root->content.If.elseBody[j]);
        }
        if(root->content.If.elseBody[j]->nodeType == AST_ASSIGN){
            if(root->content.If.elseBody[j]->content.assign.exprCnt > 1){
                for(int k = 0 ; k < root->content.If.elseBody[j]->content.assign.exprCnt;k++){
                    if(strcmp(root->content.If.elseBody[j]->content.assign.IDs[k]->attribute.string,"_") != 0){
                        DEFFREEVAR
                        tmpDef++;
                        TMPVAR(tmpDef);
                        NEWLINE
                    }
                }
            }
        }
    }
}

/**
 * @brief generate ifj20code output
 * @param root AST subtree node
 * @param symTables 
 * @return int status code
 */
int codeGenerate(ASTNode *root,tList *symTables){

    if(root == NULL){
        return SEMANTIC_OTHER_ERR;
    }    
    switch (root->nodeType) {
        case AST_PROG:
        ///moved into generatorInit() 
        break;
        case AST_FUNC:
            ///push function symTable
            DLInsertFirst(symTables, (TablePtr) root->content.func.table);

            // TODO : Remove from this case to func call and create struct to generate func

            if(strcmp(root->content.func.ID->attribute.string,"main") == 0){
                mainRet = 1;
                funcGenerate(root->content.func.ID->attribute.string,true);
            } else {
                mainRet = 0;
                funcGenerate(root->content.func.ID->attribute.string,false);
            }
            // create returnValues inside function
            for(int i = 0; i < root->content.func.retTypesCnt;i++){

                fprintf(stdout,"DEFVAR LF@returnValue%d\n",i);
                fprintf(stdout,"MOVE LF@returnValue%d nil@nil\n",i);
            }
            // create funcParams inside function
            for(int i = 0; i < root->content.func.paramsCnt;i++){
                DEFFREEVAR
                tokenGenerate(root->content.func.params[i]->content.funcParam.ID, symTables);
                NEWLINE
                MOVEFREE
                tokenGenerate(root->content.func.params[i]->content.funcParam.ID, symTables);
                fprintf(stdout,"LF@arg%d\n",i);
                ///add func params to the deflist and set it to inited
                int scope = 0;
                findSymbol(root->content.func.params[i]->content.funcParam.ID->attribute.string, symTables, &scope);
                pushSymbol(SCOPE-scope, root->content.func.params[i]->content.funcParam.ID->attribute.string);
                symbolInit(root->content.func.params[i]->content.funcParam.ID, symTables);
            }
             // do the magic
            for(int i = 0 ; i < root->content.func.bodyCnt;i++){
                if (root->content.func.body[i]->nodeType == AST_IF) {
                    defVarsInsideIf(root->content.func.body[i]);
                }
                if(root->content.func.body[i]->nodeType == AST_FOR){
                    defvarsInsideFor(root->content.func.body[i]);
                }
                if(root->content.func.body[i]->nodeType == AST_ASSIGN){
                    if(root->content.func.body[i]->content.assign.exprCnt > 1){
                        for(int j = 0 ; j < root->content.func.body[i]->content.assign.exprCnt;j++){
                            if(strcmp(root->content.func.body[i]->content.assign.IDs[j]->attribute.string,"_") != 0){
                                DEFFREEVAR
                                tmpDef++;
                                TMPVAR(tmpDef);
                                NEWLINE
                            }
                        }
                    }   
                }
            }
            for (int i = 0; i < root->content.func.bodyCnt; i++) { 
                codeGenerate(root->content.func.body[i], symTables);
            }
            if(strcmp(root->content.func.ID->attribute.string,"main") == 0){
                fprintf(stdout,"POPFRAME\n"); // main without return
                fprintf(stdout,"EXIT int@0\n");
            }
            ///pop function symTable
            DLDeleteFirst(symTables);
            ///clear all defVars
            clearSymbols();
            DLDisposeSafe(defList);
            break;
        case AST_FUNCCALL:
            CREATEFRAME
            for(int i = 0; i < root->content.funcCall.argsCnt;i++){   
                DEFFREEVAR
                fprintf(stdout,"TF@arg%d\n",i);
                MOVEFREE
                fprintf(stdout,"TF@arg%d ",i);
                tokenGenerateToAssign(root->content.funcCall.args[i], symTables);
                NEWLINE
            }
            CALLFREE
            if(root->content.funcCall.ID->type == TOKEN_TYPE_IDENTIFIER){
                fprintf(stdout,"%s",root->content.funcCall.ID->attribute.string);
                if(strcmp(root->content.funcCall.ID->attribute.string,"print") == 0){
                    printCounter++;
                    fprintf(stdout,"%d",printCounter);
                }
            }
            NEWLINE
            if(strcmp(root->content.funcCall.ID->attribute.string,"inputs") == 0){
                bInputs++;
            }else if (strcmp(root->content.funcCall.ID->attribute.string,"inputi") == 0){
                bInputi++;
            }else if(strcmp(root->content.funcCall.ID->attribute.string,"inputf") == 0){
                bInputf++;
            }else if(strcmp(root->content.funcCall.ID->attribute.string,"print") == 0){
                printParamsCount[bPrint] = root->content.funcCall.argsCnt; // count terms
                bPrint++;
            }else if(strcmp(root->content.funcCall.ID->attribute.string,"int2float") == 0){
                bInt2float++;
            }else if(strcmp(root->content.funcCall.ID->attribute.string,"float2int") == 0){
                bFloat2int++;
            }else if(strcmp(root->content.funcCall.ID->attribute.string,"len") == 0){
                bLen++;
            }else if(strcmp(root->content.funcCall.ID->attribute.string,"substr") == 0){
                bSubstr++;
            }else if(strcmp(root->content.funcCall.ID->attribute.string,"ord") == 0){
                bOrd++;
            }else if(strcmp(root->content.funcCall.ID->attribute.string,"chr") == 0){
                bChr++;
            }
            break;
        case AST_RET:
            if(mainRet > 0){
                fprintf(stdout,"POPFRAME\n"); // main without return
                fprintf(stdout,"EXIT int@0\n");
            } else{
                   // move variables to return values
                for(int i = 0 ; i < root->content.ret.exprCnt;i++){
                    if(root->content.ret.multiExpr[i]->nodeType == AST_LIT){
                        MOVEFREE
                        fprintf(stdout,"LF@returnValue%d ",i);
                        tokenGenerate(root->content.ret.multiExpr[i]->content.lit.token, symTables);
                        NEWLINE 
                    } else if (root->content.ret.multiExpr[i]->nodeType == AST_EXPR){
                        exprGenerate(root->content.ret.multiExpr[i], symTables);
                        POPSFREE
                        fprintf(stdout,"LF@returnValue%d",i);
                        NEWLINE
                    }
                }
            FUNCEND
            }
            break;
        case AST_IF:
            ifGenerate(root, symTables);
            break;
        case AST_FOR:
            forGenerate(root, symTables);
            break;
        case AST_DEC:
            if (!isSymbDef(root->content.declare.ID, symTables)) {
                DEFFREEVAR
                tokenGenerate(root->content.declare.ID, symTables);
                NEWLINE
                int scope = 0;
                findSymbol(root->content.declare.ID->attribute.string, symTables, &scope);
                pushSymbol((SCOPE-scope), root->content.declare.ID->attribute.string);
            }

             if(root->content.declare.expr->nodeType == AST_LIT){
                MOVEFREE
                tokenGenerate(root->content.declare.ID, symTables);
                tokenGenerateToAssign(root->content.declare.expr->content.lit.token, symTables);
                NEWLINE
                symbolInit(root->content.declare.ID, symTables);
            } else if(root->content.declare.expr->nodeType == AST_EXPR){
                exprGenerate(root->content.declare.expr, symTables);
                POPSFREE
                tokenGenerate(root->content.declare.ID, symTables);
                NEWLINE
                symbolInit(root->content.declare.ID, symTables);
            }
            break;
       case AST_ASSIGN:
            if(root->content.assign.multiExpr[0]->nodeType == AST_FUNCCALL){
                codeGenerate(root->content.assign.multiExpr[0], symTables);
                for(int i = 0 ; i < root->content.assign.IDCnt;i++){
                    // dont accept _
                    if(strcmp(root->content.assign.IDs[i]->attribute.string,"_") != 0){
                        MOVEFREE
                        tokenGenerate(root->content.assign.IDs[i], symTables);
                        fprintf(stdout,"TF@returnValue%d\n",i);
                    }
                }
            } else {
                if(root->content.assign.exprCnt == 1){
                    if(strcmp(root->content.assign.IDs[0]->attribute.string,"_") != 0) {
                        if(root->content.assign.multiExpr[0]->nodeType == AST_LIT){
                            MOVEFREE
                            tokenGenerate(root->content.assign.IDs[0], symTables);
                            tokenGenerateToAssign(root->content.assign.multiExpr[0]->content.lit.token, symTables);
                            NEWLINE
                        } else if (root->content.assign.multiExpr[0]->nodeType == AST_EXPR){
                            exprGenerate(root->content.assign.multiExpr[0], symTables);
                            POPSFREE 
                            tokenGenerateToAssign(root->content.assign.IDs[0], symTables);
                            NEWLINE
                        }
                    }     
                } else {
                    int IDCounter = tmpCounter;
                    for(int i = 0; i < root->content.assign.exprCnt;i++){
                        if(strcmp(root->content.assign.IDs[i]->attribute.string,"_") != 0){
                            if(root->content.assign.multiExpr[i]->nodeType == AST_LIT){
                                MOVEFREE
                                TMPVAR(tmpCounter);
                                SPACE
                                tokenGenerateToAssign(root->content.assign.multiExpr[i]->content.lit.token, symTables);
                                NEWLINE
                                tmpCounter++;
                            } else {
                            exprGenerate(root->content.assign.multiExpr[i], symTables);
                            POPSFREE
                            TMPVAR(tmpCounter);
                            NEWLINE
                            tmpCounter++;
                            }
                        }
                    }
                    int tmpVarCount = 0;
                    for(int j = 0; j < root->content.assign.exprCnt;j++){
                        if(strcmp(root->content.assign.IDs[j]->attribute.string,"_") != 0){
                            MOVEFREE
                            tokenGenerate(root->content.assign.IDs[j], symTables);
                            SPACE
                            TMPVAR(tmpVarCount+IDCounter);
                            NEWLINE
                            tmpVarCount++;
                        }
                    }
                }
            }
            break;
        case AST_EXPR:
            exprGenerate(root, symTables);
            break;
        default:
            break;
    }
    return SUCCESS;
}