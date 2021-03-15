/**
 * @file scanner.h
 * @author Daniel Gavenda (xgaven08@stud.fit.vutbr.cz)
 * @author Samuel Valaštín (xvalas10@stud.fit.vutbr.cz)
 * @brief DFSM for lexical analysis
 * @date 2020-10-10
 * 
 */

#ifndef scannerHeader
#define scannerHeader

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include "const.h"
#include "error.h"
#include "myString.h"
#include <stdint.h>

typedef enum {
    SCANNER_STATE_START,                
    SCANNER_STATE_NUMBER,
    SCANNER_STATE_NUMBER_ZERO,
    SCANNER_STATE_DECIMAL_POINT,
    SCANNER_STATE_DECIMAL_PART,
    SCANNER_STATE_EXPONENT,
    SCANNER_STATE_EXPONENT_SIGN,
    SCANNER_STATE_EXPONENT_NUMBER,
    SCANNER_STATE_IDENTIF_OR_KEYWORD,
    SCANNER_STATE_OP_DIV,
    SCANNER_STATE_COMMENTARY_LINE,
    SCANNER_STATE_COMMENTARY_BLOCK,
    SCANNER_STATE_COMMENTARY_BLOCK_END,
    SCANNER_STATE_REL_GT,
    SCANNER_STATE_REL_LT,
    SCANNER_STATE_ASSIGN,
    SCANNER_STATE_NOT,
    SCANNER_STATE_COLON,
    SCANNER_STATE_STRING,
    SCANNER_STATE_ESCAPE_SEQUENCE,
    SCANNER_STATE_HEXAONE,
    SCANNER_STATE_HEXATWO,
    SCANNER_STATE_EOL
} State;

typedef enum {
    TOKEN_TYPE_EMPTY,           // 0
    TOKEN_TYPE_EOL,             // 1
    TOKEN_TYPE_EOF,             // 2
    TOKEN_TYPE_IDENTIFIER,      // 3

    /// data types
    TOKEN_TYPE_INTEGER,         // 4
    TOKEN_TYPE_DOUBLE,          // 5
    TOKEN_TYPE_STRING,          // 6

    ///relation operators
    TOKEN_TYPE_LT, // <         // 7
    TOKEN_TYPE_LTE, // <=       // 8
    TOKEN_TYPE_GT, // >         // 9
    TOKEN_TYPE_GTE, // >=       // 10
    TOKEN_TYPE_EQ, // ==        // 11
    TOKEN_TYPE_NEQ, // !=       // 12

    ///operators
    TOKEN_TYPE_DECLARE, // :=   // 13
    TOKEN_TYPE_ASSIGN, // =     // 14
    TOKEN_TYPE_ADD, //  +       // 15
    TOKEN_TYPE_SUB, //  -       // 16
    TOKEN_TYPE_MUL, //  *       // 17
    TOKEN_TYPE_DIV, //  /       // 18

    TOKEN_TYPE_LEFT_PARENTHESE, // 19
    TOKEN_TYPE_RIGHT_PARENTHESE,// 20
    TOKEN_TYPE_LEFT_BRACKET,    // 21
    TOKEN_TYPE_RIGHT_BRACKET,   // 22
    TOKEN_TYPE_COMMA, // ,      // 23
    TOKEN_TYPE_SEMICOLON, // ;  // 24

    ///Keywords, as token types
    TOKEN_TYPE_KEYWORD_IF,      // 25
    TOKEN_TYPE_KEYWORD_ELSE,    // 26
    TOKEN_TYPE_KEYWORD_INT,     // 27
    TOKEN_TYPE_KEYWORD_FLOAT64, // 28
    TOKEN_TYPE_KEYWORD_STRING,  // 29
    TOKEN_TYPE_KEYWORD_FOR,     // 30
    TOKEN_TYPE_KEYWORD_FUNC,    // 31
    TOKEN_TYPE_KEYWORD_RETURN,  // 32
    TOKEN_TYPE_KEYWORD_PACKAGE, // 33

    //Mirdovo gerbič
    TOKEN_TYPE_BOOL,            // 34
} TokenType;

typedef union {
    char *string;          // string or identifier
    int64_t integer;           // int value 
    double float64;        // scientific value
} TokenAttribute;

typedef struct{
    TokenType type;
    TokenAttribute attribute;
} Token;

/**
 * 
 * Returns a token from the source file 
 * 
 */
int getToken(Token *token);

/**
 * sets source code file
 */
void setTokenSrc(FILE *fd);

#endif
