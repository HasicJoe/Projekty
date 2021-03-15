/**
 * @file grammar.h
 * @author Jonáš Tichý (xtichy29@stud.fit.vutbr.cz)
 * @author Miroslav Štěpánek (xstepa68@stud.fit.vutbr.cz)
 * @brief IFJ20 Grammar represented by Nodes for recursive descend
 * @date 2020-10-26
 * 
 */

// Cannot be included directly in parser.h because of declaration duplication errors
#include "parser.h"
#include "scanner.h"

GrammarNode _package = {.type = LEAF, .content.TokenType = TOKEN_TYPE_KEYWORD_PACKAGE};
GrammarNode _empty = {.type = LEAF, .content.TokenType = TOKEN_TYPE_EMPTY};
GrammarNode _eol = {.type = LEAF, .content.TokenType = TOKEN_TYPE_EOL};
GrammarNode _eof = {.type = LEAF, .content.TokenType = TOKEN_TYPE_EOF};
GrammarNode _id = {.type = LEAF, .content.TokenType = TOKEN_TYPE_IDENTIFIER};
GrammarNode _iLit = {.type = LEAF, .content.TokenType = TOKEN_TYPE_INTEGER};
GrammarNode _fLit = {.type = LEAF, .content.TokenType = TOKEN_TYPE_DOUBLE};
GrammarNode _sLit = {.type = LEAF, .content.TokenType = TOKEN_TYPE_STRING};
GrammarNode _integer = {.type = LEAF, .content.TokenType = TOKEN_TYPE_KEYWORD_INT};
GrammarNode _float = {.type = LEAF, .content.TokenType = TOKEN_TYPE_KEYWORD_FLOAT64};
GrammarNode _string = {.type = LEAF, .content.TokenType = TOKEN_TYPE_KEYWORD_STRING};
GrammarNode _declare = {.type = LEAF, .content.TokenType = TOKEN_TYPE_DECLARE};
GrammarNode _assign = {.type = LEAF, .content.TokenType = TOKEN_TYPE_ASSIGN};
GrammarNode _lParenth = {.type = LEAF, .content.TokenType = TOKEN_TYPE_LEFT_PARENTHESE};
GrammarNode _rParenth = {.type = LEAF, .content.TokenType = TOKEN_TYPE_RIGHT_PARENTHESE};
GrammarNode _lBracket = {.type = LEAF, .content.TokenType = TOKEN_TYPE_LEFT_BRACKET};
GrammarNode _rBracket = {.type = LEAF, .content.TokenType = TOKEN_TYPE_RIGHT_BRACKET};
GrammarNode _comma = {.type = LEAF, .content.TokenType = TOKEN_TYPE_COMMA};
GrammarNode _semicolon = {.type = LEAF, .content.TokenType = TOKEN_TYPE_SEMICOLON};
GrammarNode _return = {.type = LEAF, .content.TokenType = TOKEN_TYPE_KEYWORD_RETURN};
GrammarNode _if = {.type = LEAF, .content.TokenType = TOKEN_TYPE_KEYWORD_IF};
GrammarNode _func = {.type = LEAF, .content.TokenType = TOKEN_TYPE_KEYWORD_FUNC};
GrammarNode _else = {.type = LEAF, .content.TokenType = TOKEN_TYPE_KEYWORD_ELSE};
GrammarNode _for = {.type = LEAF, .content.TokenType = TOKEN_TYPE_KEYWORD_FOR};
GrammarNode _mainId = {.type = LEAF, .content.TokenType = TOKEN_TYPE_IDENTIFIER};

// Forward declaration due to cyclic dependency - need to be linked at runtime
GrammarNode If;
GrammarNode For;

// Eols + eps
GrammarNode eps = {.type = NODE, .parseType = PT_EPS, .content.branches.opts = {{NULL}}, .content.branches.optCnt = 1, .content.branches.optSize = {0}};
GrammarNode eolOpt = {.type = NODE, .parseType = PT_EOL_OPT, .content.branches.opts = {{&_eol}, {&eps}}, .content.branches.optCnt = 2, .content.branches.optSize = {1,1}};
GrammarNode multiEol = {.type = NODE, .parseType = PT_MULTI_EOL, .content.branches.opts = {{&_eol, &multiEol}, {&eps}}, .content.branches.optCnt = 2, .content.branches.optSize = {2,1}};

GrammarNode lParenthsOpt = {.type = NODE, .parseType = PT_L_PARENTH_OPT, .content.branches.opts = {{&_lParenth, &multiEol,&lParenthsOpt}, {&eps}}, .content.branches.optCnt = 2, .content.branches.optSize = {3,1}};
GrammarNode rParenthsOpt = {.type = NODE, .parseType = PT_R_PARENTH_OPT, .content.branches.opts = {{&_rParenth, &rParenthsOpt}, {&eps}}, .content.branches.optCnt = 2, .content.branches.optSize = {2,1}};
GrammarNode type = {.type = NODE, .parseType = PT_TYPE, .content.branches.opts = {{&_integer}, {&_string}, {&_float}}, .content.branches.optCnt = 3, .content.branches.optSize = {1,1,1}};
GrammarNode operand = {.type = NODE, .parseType = PT_OPERAND, .content.branches.opts = {{&_id}, {&_iLit}, {&_fLit}, {&_sLit}}, .content.branches.optCnt = 4, .content.branches.optSize = {1,1,1,1}};
GrammarNode multId = {.type = NODE, .parseType = PT_MULTI_ID, .content.branches.opts = {{&_id, &_comma, &multId}, {&_id}}, .content.branches.optCnt = 2, .content.branches.optSize = {3,1}};

// func call
GrammarNode funcCallArgs = {.type = NODE, .parseType = PT_FUNC_CALL_ARGS, .content.branches.opts = {{&operand, &_comma, &funcCallArgs}, {&operand}}, .content.branches.optCnt = 2, .content.branches.optSize = {3,1}};
GrammarNode funcCall = {.type = NODE, .parseType = PT_FUNC_CALL, .content.branches.opts = {{&_id, &_lParenth, &funcCallArgs, &_rParenth}, {&_id, &_lParenth, &_rParenth}}, .content.branches.optCnt = 2, .content.branches.optSize = {4,3}};

GrammarNode expr = {.type = NODE, .parseType = PT_EXPR, .content.branches.opts = {{NULL}}, .content.branches.optCnt = 1, .content.branches.optSize = {0}};
GrammarNode multiExpr = {.type = NODE, .parseType = PT_MULTIEXPR, .content.branches.opts = {{&expr, &_comma, &multiExpr}, {&expr}}, .content.branches.optCnt = 2, .content.branches.optSize = {3,1}};
GrammarNode define = {.type = NODE, .parseType = PT_DEFINE, .content.branches.opts = {{&_id, &_declare, &eolOpt, &expr}}, .content.branches.optCnt = 1, .content.branches.optSize = {4}};
GrammarNode ret = {.type = NODE, .parseType = PT_RET, .content.branches.opts = {{&_return, &multiExpr}, {&_return}}, .content.branches.optCnt = 2, .content.branches.optSize = {2,1}};
GrammarNode assign = {.type = NODE, .parseType = PT_ASSIGN, .content.branches.opts = {{&multId, &_assign, &eolOpt, &funcCall}, {&multId, &_assign, &eolOpt, &multiExpr}}, .content.branches.optCnt = 2, .content.branches.optSize = {4,4}};
GrammarNode statement = {.type = NODE, .parseType = PT_STATEMENT, .content.branches.opts = {{&If}, {&For}, {&define}, {&assign}, {&funcCall}, {&ret}, {&eps}}, .content.branches.optCnt = 7, .content.branches.optSize = {1,1,1,1,1,1,1}};
GrammarNode body = {.type = NODE, .parseType = PT_BODY, .content.branches.opts = {{&statement, &_eol, &body}, {&eps}}, .content.branches.optCnt = 2, .content.branches.optSize = {3,1}};
GrammarNode holderIf = {.type = NODE, .parseType = PT_IF, .content.branches.opts = {{&_if, &expr, &_lBracket, &_eol, &body, &_rBracket, &_else, &_lBracket, &_eol, &body, &_rBracket}}, .content.branches.optCnt = 1, .content.branches.optSize = {11}};

// func
GrammarNode params = {.type = NODE, .parseType = PT_PARAMS, .content.branches.opts = {{&_id, &type, &_comma, &params}, {&_id, &type}}, .content.branches.optCnt = 2, .content.branches.optSize = {4,2}};
GrammarNode retTypes = {.type = NODE, .parseType = PT_RET_TYPES, .content.branches.opts = {{&type, &_comma, &retTypes}, {&type}}, .content.branches.optCnt = 2, .content.branches.optSize = {3,1}};
GrammarNode funcParams = {.type = NODE, .parseType = PT_FUNC_PARAMS, .content.branches.opts = {{&_lParenth, &params, &_rParenth}, {&_lParenth, &_rParenth}}, .content.branches.optCnt = 2, .content.branches.optSize = {3,2}};
GrammarNode funcRetTypes = {.type = NODE, .parseType = PT_FUNC_RET_TYPES, .content.branches.opts = {{&_lParenth, &retTypes, &_rParenth}, {&_lParenth, &_rParenth}, {&eps}}, .content.branches.optCnt = 3, .content.branches.optSize = {3,2,1}};
GrammarNode func = {.type = NODE, .parseType = PT_FUNC, .content.branches.opts = {{&_func, &_id, &funcParams, &funcRetTypes, &_lBracket, &_eol, &body, &_rBracket}}, .content.branches.optCnt = 1, .content.branches.optSize = {8}};

// for
GrammarNode forDef = {.type = NODE, .parseType = PT_FOR_DEC, .content.branches.opts = {{&define}, {&eps}}, .content.branches.optCnt = 2, .content.branches.optSize = {1,1}};
GrammarNode forInc = {.type = NODE, .parseType = PT_FOR_INC, .content.branches.opts = {{&assign}, {&eps}}, .content.branches.optCnt = 2, .content.branches.optSize = {1,1}};
GrammarNode holderFor = {.type = NODE, .parseType = PT_FOR, .content.branches.opts = {{&_for, &forDef, &_semicolon, &expr, &_semicolon, &forInc, &_lBracket, &_eol, &body, &_rBracket}}, .content.branches.optCnt = 1, .content.branches.optSize = {10}};

// prog
GrammarNode progBody = {.type = NODE, .parseType = PT_PROG_BODY,.content.branches.opts = {{&multiEol, &func, &progBody}, {&multiEol}}, .content.branches.optCnt = 2, .content.branches.optSize = {3,1}};
GrammarNode prog = {.type = NODE,.parseType = PT_PROG, .content.branches.opts = {{&multiEol, &_package, &multiEol, &_mainId, &_eol, &progBody}}, .content.branches.optCnt = 1, .content.branches.optSize = {6}};
