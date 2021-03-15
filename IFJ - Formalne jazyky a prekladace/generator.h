/**
 * @file generator.h
 * @author Samuel Valaštín (xvalas10@stud.fit.vutbr.cz)
 * @author Daniel Gavenda (xgaven08@stud.fit.vutbr.cz)
 * @brief Generator of three address code
 * @date 2020-11-18
 * 
 */

#ifndef _GENERATOR_H
#define _GENERATOR_H 1

#include <stdio.h>
#include <string.h>
#include "parser.h"
#include "error.h"
#include "list.h"


#define INSTR(instruction) fprintf(stdout, instruction);

///header of ifj20code program
#define GENERATE_HEADER fprintf(stdout,".IFJcode20\nJUMP main\n");

#define FUNCEND        fprintf(stdout,"POPFRAME\nRETURN\n");

///print scope
#define pSCOPE(tag) printf("\n%s scope: %d\n\n",tag, countList(symTables));
#define SCOPE countList(symTables)
///INSTRUCTION_SET
#define MOVE(var, symb)             fprintf(stdout, "MOVE %s %s\n",var ,symb);
#define CREATEFRAME                 fprintf(stdout, "CREATEFRAME\n");
#define PUSHFRAME                   fprintf(stdout, "PUSHFRAME\n");
#define POPFRAME                    fprintf(stdout, "POPFRAME\n");
#define DEFVAR(var)                 fprintf(stdout, "DEFVAR %s\n", var);
#define CALL(label)                 fprintf(stdout, "CALL %s\n", label);
#define RETURN                      fprintf(stdout, "RETURN\n");
#define RETVALUE(count)             fprintf(stdout,"LF@returnValue%d ",count);
#define MAINEND                     fprintf(stdout,"POPFRAME\nEXIT int@0\n");
#define IFDEFVAR(count)             fprintf(stdout, "DEFVAR LF@ifResult%d\n", count);
#define TFRETVALUE(count)           fprintf(stdout,"LF@returnValue%d ",count);
#define TFARG(count)                fprintf(stdout,"TF@arg%d ",count);
#define NILNIL                      fprintf(stdout,"nil@nil");
#define SPACE                       fprintf(stdout," ");
#define TMPVAR(var)                 fprintf(stdout,"LF@tmpVar%d",var)

#define DEFFREEVAR                  fprintf(stdout,"DEFVAR ");
#define NEWLINE                     fprintf(stdout,"\n");
#define JUMPFREE                    fprintf(stdout,"JUMP ");
#define MOVEFREE                    fprintf(stdout,"MOVE ");
#define CALLFREE                    fprintf(stdout,"CALL ");
#define POPSFREE                    fprintf(stdout,"POPS ");
#define PUSHSFREE                   fprintf(stdout, "PUSHS ");

#define PUSHS(symb)                 fprintf(stdout, "PUSHS %s\n", symb);
#define POPS(var)                   fprintf(stdout, "POPS %s\n", var);
#define CLEARS                      fprintf(stdout, "CLEARS\n");

#define ADD(var, symb1, symb2)      fprintf(stdout, "ADD %s %s %s\n", var, symb1, symb2);
#define SUB(var, symb1, symb2)      fprintf(stdout, "SUB %s %s %s\n", var, symb1, symb2);
#define MUL(var, symb1, symb2)      fprintf(stdout, "MUL %s %s %s\n", var, symb1, symb2);
#define DIV(var, symb1, symb2)      fprintf(stdout, "DIV %s %s %s\n", var, symb1, symb2);
#define IDIV(var, symb1, symb2)     fprintf(stdout, "IDIV %s %s %s\n", var, symb1, symb2);

#define ADDS                        fprintf(stdout, "ADDS\n");
#define SUBS                        fprintf(stdout, "SUBS\n");
#define MULS                        fprintf(stdout, "MULS\n");
#define DIVS                        fprintf(stdout, "DIVS\n");
#define IDIVS                       fprintf(stdout, "IDIVS\n");

#define LT(var, symb1, symb2)       fprintf(stdout, "LT %s %s %s\n", var, symb1, symb2);
#define GT(var, symb1, symb2)       fprintf(stdout, "GT %s %s %s\n", var, symb1, symb2);
#define EQ(var, symb1, symb2)       fprintf(stdout, "EQ %s %s %s\n", var, symb1, symb2);
#define LTFREE                      fprintf(stdout,"LT ");
#define GTFREE                      fprintf(stdout,"GT ");
#define EQFREE                      fprintf(stdout,"EQ ");

#define LTS                         fprintf(stdout, "LTS\n");
#define GTS                         fprintf(stdout, "GTS\n");
#define EQS                         fprintf(stdout, "EQS\n");

#define AND(var, symb1, symb2)      fprintf(stdout, "AND %s %s %s\n", var, symb1, symb2);
#define OR(var, symb1, symb2)       fprintf(stdout, "OR %s %s %s\n", var, symb1, symb2);
#define NOT(var, symb)              fprintf(stdout, "NOT %s %s\n", var, symb);

#define ANDS                        fprintf(stdout, "ANDS\n");
#define ORS                         fprintf(stdout, "ORS\n");
#define NOTS                        fprintf(stdout, "NOTS\n");

#define INT2FLOAT(var, symb)        printf(stdout, "INT2FLOAT %s %s\n", var, symb);
#define FLOAT2INT(var, symb)        printf(stdout, "FLOAT2INT %s %s\n", var, symb);
#define INT2CHAR(var, symb)         printf(stdout, "INT2CHAR %s %s\n", var, symb);
#define STRI2INT(var, symb)         printf(stdout, "STRI2INT %s %s\n", var, symb);

#define INT2FLOATS                  fprintf(stdout, "INT2FLOATS\n");
#define FLOAT2INTS                  fprintf(stdout, "FLOAT2INTS\n");
#define INT2CHARS                   fprintf(stdout, "INT2CHARS\n");
#define STRI2INTS                   fprintf(stdout, "STRI2INTS\n");

#define READ(var, type)             fprintf(stdout, "READ %s %s\n",var, type);
#define WRITE(symb)                 fprintf(stdout, "WRITE %s\n", symb);

#define CONCAT(var, symb1, symb2)   fprintf(stdout, "CONCAT %s %s %s\n", var, symb1, symb2);
#define STRLEN(var, symb)           fprintf(stdout, "STRLEN %s %s\n", var, symb);
#define GETCHAR(var, symb1, symb2)  fprintf(stdout, "GETCHAR %s %s %s\n", var, symb1, symb2);
#define SETCHAR(var, symb1, symb2)  fprintf(stdout, "SETCHAR %s %s %s\n", var, symb1, symb2);

#define TYPE(var, symb)             fprintf(stdout, "TYPE %s %s\n", var, symb);
#define LABEL(label)                fprintf(stdout, "LABEL %s\n", label);
#define JUMP(label)                 fprintf(stdout, "JUMP %s\n", label);

#define JUMPIFEQ(label, symb1, symb2) fprintf(stdout, "JUMPIFEQ %s %s %s\n", label symb1, symb2);
#define JUMPIFNEQ(label, symb1, symb2) fprintf(stdout, "JUMPIFNEQ %s %s %s\n", label symb1, symb2);

#define JUMPIFEQS(label)            fprintf(stdout, "JUMPIFEQS %s\n", label);
#define JUMPISNEQS(label)           fprintf(stdout, "JUMPIFNEQS %s\n", label);

#define EXIT(symb)                  fprintf(stdout, "EXIT %s\n", symb);

#define BREAK                       fprintf(stdout, "BREAK\n");
#define DPRINT                      fprintf(stdout, "DPRINT\n");

#define INPUTS 0
#define INPUTI 1
#define INPUTF 2
#define PRINT 3
#define INTTOFLOAT 4
#define FLOATTOINT 5
#define LEN 6
#define SUBSTR 7
#define ORD 8
#define CHR 9
#define MAX_SCOPE 64

typedef struct {
    char *name;
    int scope;
    bool init;
} Symb;

void printSymbols();
void symbolInit(Token *token, tList *symTables);
int getSymbolScope(char *id, tList *symTables);
int generatorInit(ASTNode *root);
int codeGenerate(ASTNode*, tList* symTables);
void forAssGen(ASTNode *root,tList *symTables);
Item *findSymbol(char *id, tList *symTables, int *scope);
void pushSymbol(int scope, char *name);
bool isSymbDef(Token *token, tList *symTables);
void exprGenerate(ASTNode *root, tList *symTables);
void funcGenerate(char *,bool);
void tokenGenerate(Token*, tList *symTables);
void inputsGenerate();
void inputiGenerate();
void inputfGenerate();
void concatGenerate();
void printGenerate(int termCounter,int identifier);
void int2floatGenerate();
void float2intGenerate();
void lenGenerate();
void substrGenerate();
void ordGenerate();
void chrGenerate();
void ifGenerate(ASTNode *, tList *symTables);
void IDGenerate(Token * token);
void bulletInGenerate();
void funcEndGenerate();
void defVarsInsideIf(ASTNode *root);
void defVarsInsideFor(ASTNode *root);
#endif