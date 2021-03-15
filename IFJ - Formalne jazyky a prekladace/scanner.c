/**
 * @file scanner.c
 * @author Daniel Gavenda (xgaven08@stud.fit.vutbr.cz)
 * @author Samuel Valaštín (xvalas10@stud.fit.vutbr.cz)
 * @brief DFSM for lexical analysis
 * @date 2020-10-10
 * 
 */

#include "scanner.h"

/**
 * @brief Set the Integer object
 * 
 * @param bufferPtr 
 * @param token 
 * @return int 
 */
int setInteger(MyString *bufferPtr, Token *token) {
    char *terminate = NULL;

    int64_t intValue = strtol(bufferPtr->data, &terminate,10);

    if (*terminate != '\0') {
        myStrFree(bufferPtr);
        return LEXICAL_ERR;
    }

    token->attribute.integer = intValue;
    token->type = TOKEN_TYPE_INTEGER;

    myStrFree(bufferPtr);
    return STATUS_OK;
}


/**
 * @brief sets token type to FLOAT and its attribute
 * 
 * @param bufferPtr value in string
 * @param token 
 * @return int status
 */
int setFloat64(MyString *bufferPtr, Token *token) {
    char *terminate = NULL;

    double float64Value = strtod(bufferPtr->data, &terminate);
    if (*terminate != '\0') {
        myStrFree(bufferPtr);
        return LEXICAL_ERR;
    }
    
    token->attribute.float64 = float64Value;
    token->type = TOKEN_TYPE_DOUBLE;

    myStrFree(bufferPtr);
    return STATUS_OK;
}

/**
 * @brief Set the Token Type 
 * 
 * @param token 
 * @param type 
 * @param bufferPtr 
 * @return int 
 */
int setTokenType(Token *token, TokenType type, MyString *bufferPtr) {
    token->type = type;
    myStrFree(bufferPtr);
    return STATUS_OK;
}
/**
 * @brief Set the Token String 
 * 
 * @param token 
 * @param buffer 
 * @return int 
 */
int setTokenString(Token *token, MyString *buffer) {
    token->attribute.string = (char*) malloc(strlen(buffer->data)+1);
    if (token->attribute.string == NULL) {
        return ALLOC_ERR;
    }
    strcpy(token->attribute.string, buffer->data);
    return STATUS_OK;
}

 /**
  * @brief Sets token attribute and type to keyword or identifier depends @bufferPtr matches any known keywords
  * @param bufferPtr 
  * @param token 
  * @return int 
  */
int setIdentifier(MyString *bufferPtr, Token *token)  {
    if (!myStrCmp(bufferPtr, "else")) {
        return setTokenType(token, TOKEN_TYPE_KEYWORD_ELSE, bufferPtr);
    } else if (!myStrCmp(bufferPtr, "float64")) {
        return setTokenType(token, TOKEN_TYPE_KEYWORD_FLOAT64, bufferPtr);
    } else if (!myStrCmp(bufferPtr, "for")) {
        return setTokenType(token, TOKEN_TYPE_KEYWORD_FOR, bufferPtr);
    } else if (!myStrCmp(bufferPtr, "func")) {
        return setTokenType(token, TOKEN_TYPE_KEYWORD_FUNC, bufferPtr);
    } else if (!myStrCmp(bufferPtr, "if")) {
        return setTokenType(token, TOKEN_TYPE_KEYWORD_IF, bufferPtr);
    } else if (!myStrCmp(bufferPtr, "int")) {
        return setTokenType(token, TOKEN_TYPE_KEYWORD_INT, bufferPtr);
    } else if (!myStrCmp(bufferPtr, "package")) {
        return setTokenType(token, TOKEN_TYPE_KEYWORD_PACKAGE, bufferPtr);
    } else if (!myStrCmp(bufferPtr, "return")) {
        return setTokenType(token, TOKEN_TYPE_KEYWORD_RETURN, bufferPtr);
    } else if (!myStrCmp(bufferPtr, "string")) {
        return setTokenType(token, TOKEN_TYPE_KEYWORD_STRING, bufferPtr);
    } else {
        token->type = TOKEN_TYPE_IDENTIFIER;
    }
    if (setTokenString(token, bufferPtr) == ALLOC_ERR) {
        myStrFree(bufferPtr);
        return INTERNAL_ERR;
    }

    myStrFree(bufferPtr);
    return STATUS_OK;
}

/// just for testing, will be replaced by stdin in a finished result
FILE *file;

void setTokenSrc(FILE* fd) {
    file = fd;
}

/**
 * @brief finite state machine (scanner)
 * 
 * @param token next token 
 * @return int status
 */
int getToken(Token *token) {
    // Initialization of the string buffer
    MyString buffer;
    MyString *bufferPtr = &buffer;

    if (myStrInit(bufferPtr) == ALLOC_ERR) {
        myStrFree(bufferPtr);
        return INTERNAL_ERR;
    }
    ////token type init
    token->type = TOKEN_TYPE_EMPTY;


    /// initial scanner state
    State state = SCANNER_STATE_START;
    char ch;

    ///hex escape sequence buffer
    char hexbuff[3] = {0, 0, 0};

    ///state machine cycle
    while (1) {

        ch = (char) getc(file);
        switch(state) {
            
            case SCANNER_STATE_START:
                if (ch == EOL) {
                    state = SCANNER_STATE_EOL;
                } else if (isdigit(ch)) {
                    if (myStrApp(bufferPtr, ch) == ALLOC_ERR) {
                        myStrFree(bufferPtr);
                        return INTERNAL_ERR;
                    }
                    if (ch == '0') {
                        state = SCANNER_STATE_NUMBER_ZERO;
                    } else {
                        state = SCANNER_STATE_NUMBER;
                    }
                } else if (isalpha(ch) || ch == '_') {
                   if (myStrApp(bufferPtr, ch) == ALLOC_ERR) {
                       myStrFree(bufferPtr);
                        return INTERNAL_ERR;
                    }
                    state = SCANNER_STATE_IDENTIF_OR_KEYWORD;
                } else if (ch == '\"') {
                    state = SCANNER_STATE_STRING;
                } else if (ch == '(') {
                    return setTokenType(token, TOKEN_TYPE_LEFT_PARENTHESE, bufferPtr);
                } else if (ch == ')') {
                    return setTokenType(token, TOKEN_TYPE_RIGHT_PARENTHESE, bufferPtr);
                } else if (ch == '{') {
                    return setTokenType(token, TOKEN_TYPE_LEFT_BRACKET, bufferPtr);
                } else if (ch == '}') {
                    return setTokenType(token, TOKEN_TYPE_RIGHT_BRACKET, bufferPtr);
                } else if (ch == ',') {
                    return setTokenType(token, TOKEN_TYPE_COMMA, bufferPtr);
                } else if (ch == ';') {
                    return setTokenType(token, TOKEN_TYPE_SEMICOLON, bufferPtr);
                } else if (ch == ':') {
                    state = SCANNER_STATE_COLON;
                } else if (ch == '!') {
                    state = SCANNER_STATE_NOT;
                } else if (ch == '=') {
                    state = SCANNER_STATE_ASSIGN;
                } else if (ch == '<') {
                    state = SCANNER_STATE_REL_LT;
                } else if (ch == '>') {
                    state = SCANNER_STATE_REL_GT;
                } else if (ch == '+') {
                    return setTokenType(token, TOKEN_TYPE_ADD, bufferPtr);
                } else if (ch == '-') {
                    return setTokenType(token, TOKEN_TYPE_SUB, bufferPtr);
                } else if (ch == '*') {
                    return setTokenType(token, TOKEN_TYPE_MUL, bufferPtr);
                } else if (ch == '/') {
                    state = SCANNER_STATE_OP_DIV;
                } else if (ch == EOF) {
                    setTokenType(token, TOKEN_TYPE_EOF, bufferPtr);
                    return STATUS_FINISH;
                } else if (isspace(ch)) {
                    break;
                } else {
                    myStrFree(bufferPtr);
                    return LEXICAL_ERR;
                }
            break;

            case SCANNER_STATE_NUMBER_ZERO:
                if (ch == '.') {
                    if (myStrApp(bufferPtr, ch) == ALLOC_ERR) {
                        myStrFree(bufferPtr);
                        return INTERNAL_ERR;
                    }
                    state = SCANNER_STATE_DECIMAL_PART;
                } else if (isdigit(ch) || isalpha(ch)) {
                    myStrFree(bufferPtr);
                    return LEXICAL_ERR;
                } else {
                    ungetc(ch, file);
                    return setInteger(bufferPtr, token);
                }
            break;

            case SCANNER_STATE_NUMBER:
                if (isdigit(ch)) {
                    if (myStrApp(bufferPtr, ch) == ALLOC_ERR) {
                        myStrFree(bufferPtr);
                        return INTERNAL_ERR;
                    }
                } else if (ch == '.') {
                    if (myStrApp(bufferPtr, ch) == ALLOC_ERR) {
                        myStrFree(bufferPtr);
                        return INTERNAL_ERR;
                    }
                    state = SCANNER_STATE_DECIMAL_POINT;
                } else if(ch == 'e' || ch == 'E') {
                    if (myStrApp(bufferPtr, ch) == ALLOC_ERR) {
                        myStrFree(bufferPtr);
                        return INTERNAL_ERR;
                    }
                    state = SCANNER_STATE_EXPONENT;
                } else if (isalpha(ch) || ch == '_') {
                    myStrFree(bufferPtr);
                    return LEXICAL_ERR;
                } else {
                    ungetc(ch, file);
                    return setInteger(bufferPtr, token);
                }
            break;

            case SCANNER_STATE_DECIMAL_POINT:
                if (isdigit(ch)) {
                    if (myStrApp(bufferPtr, ch) == ALLOC_ERR) {
                        myStrFree(bufferPtr);
                        return INTERNAL_ERR;
                    }
                    state = SCANNER_STATE_DECIMAL_PART;
                } else {
                    myStrFree(bufferPtr);
                    return LEXICAL_ERR;
                }
            break;

            case SCANNER_STATE_DECIMAL_PART:
                if (isdigit(ch)) {
                    if (myStrApp(bufferPtr, ch) == ALLOC_ERR) {
                        myStrFree(bufferPtr);
                        return INTERNAL_ERR;
                    }
                } else if (ch == 'e' || ch == 'E') {
                    if (myStrApp(bufferPtr, ch) == ALLOC_ERR) {
                        myStrFree(bufferPtr);
                        return INTERNAL_ERR;
                    }
                    state = SCANNER_STATE_EXPONENT;
                } else if (isalpha(ch)) {
                    myStrFree(bufferPtr);
                    return LEXICAL_ERR;
                } else {
                    ungetc(ch, file);
                    return setFloat64(bufferPtr, token);
                }
            break;

            case SCANNER_STATE_EXPONENT:
                if (isdigit(ch)) {
                    if (myStrApp(bufferPtr, ch) == ALLOC_ERR) {
                        myStrFree(bufferPtr);
                        return INTERNAL_ERR;
                    }
                    state = SCANNER_STATE_EXPONENT_NUMBER;
                } else if (ch == '+' || ch == '-') {
                    if (myStrApp(bufferPtr,ch) == ALLOC_ERR) {
                        myStrFree(bufferPtr);
                        return INTERNAL_ERR;
                    }
                    state = SCANNER_STATE_EXPONENT_SIGN;
                } else {
                    myStrFree(bufferPtr);
                    return LEXICAL_ERR;
                }   
            break;

            case SCANNER_STATE_EXPONENT_SIGN:
                if (isdigit(ch)) {
                    if (myStrApp(bufferPtr, ch) == ALLOC_ERR) {
                        myStrFree(bufferPtr);
                        return INTERNAL_ERR;
                    }
                    state = SCANNER_STATE_EXPONENT_NUMBER;
                } else {
                    myStrFree(bufferPtr);
                    return LEXICAL_ERR;
                }
            break;

            case SCANNER_STATE_EXPONENT_NUMBER:
                if (isdigit(ch)) {
                    if (myStrApp(bufferPtr, ch) == ALLOC_ERR) {
                        myStrFree(bufferPtr);
                        return INTERNAL_ERR;
                    }
                } else {
                    ungetc(ch, file);
                    return setFloat64(bufferPtr, token);
                }
            break;

            case SCANNER_STATE_IDENTIF_OR_KEYWORD:
                if(isalpha(ch) || isdigit(ch) || ch == '_') {
                    if (myStrApp(bufferPtr, ch) == ALLOC_ERR) {
                        myStrFree(bufferPtr);
                        return INTERNAL_ERR;
                    }
                } else {
                    ungetc(ch, file);
                    return setIdentifier(bufferPtr, token);
                }
            break;

            case SCANNER_STATE_OP_DIV:
                if (ch == '*') {
                    state = SCANNER_STATE_COMMENTARY_BLOCK;
                } else if (ch == '/') {
                    state = SCANNER_STATE_COMMENTARY_LINE;
                } else {
                    ungetc(ch, file);
                    return setTokenType(token, TOKEN_TYPE_DIV, bufferPtr);
                }
            break;

            case SCANNER_STATE_COMMENTARY_LINE:
                if (ch == '\n') {
                    ungetc(ch, file);
                    state = SCANNER_STATE_START;
                } 
            break;

            case SCANNER_STATE_COMMENTARY_BLOCK:
                if (ch == '*') {
                    state = SCANNER_STATE_COMMENTARY_BLOCK_END;
                } else if (ch == EOF) {
                    myStrFree(bufferPtr);
                    return LEXICAL_ERR;
                }
            break;

            case SCANNER_STATE_COMMENTARY_BLOCK_END:
                if (ch == '/') {
                    state = SCANNER_STATE_START;
                } else if (ch == EOF) {
                    myStrFree(bufferPtr);
                    return LEXICAL_ERR;
                } else {
                    state = SCANNER_STATE_COMMENTARY_BLOCK;
                }
            break;

            case SCANNER_STATE_REL_GT:
                if (ch == '=') {
                    return setTokenType(token, TOKEN_TYPE_GTE, bufferPtr);
                } else {
                    ungetc(ch, file);
                    return setTokenType(token, TOKEN_TYPE_GT, bufferPtr);
                }
            break;

            case SCANNER_STATE_REL_LT:
                if (ch == '=') {
                    return setTokenType(token, TOKEN_TYPE_LTE, bufferPtr);
                } else {
                    ungetc(ch, file);
                    return setTokenType(token, TOKEN_TYPE_LT, bufferPtr);
                } 
            break;

            case SCANNER_STATE_ASSIGN:
                if (ch == '=') {
                    return setTokenType(token, TOKEN_TYPE_EQ, bufferPtr);
                } else {
                    ungetc(ch, file);
                    return setTokenType(token, TOKEN_TYPE_ASSIGN, bufferPtr);
                }
            break;

            case SCANNER_STATE_NOT:
                if (ch == '=') {
                    return setTokenType(token, TOKEN_TYPE_NEQ, bufferPtr);
                } else {
                    myStrFree(bufferPtr);
                    return LEXICAL_ERR;
                }
            break;

            case SCANNER_STATE_COLON:
                if (ch == '=') {
                    return setTokenType(token, TOKEN_TYPE_DECLARE, bufferPtr);
                } else {
                    myStrFree(bufferPtr);
                    return LEXICAL_ERR;
                }
            break;

            case SCANNER_STATE_STRING:
                if (ch < 32) {
                    myStrFree(bufferPtr);
                    return LEXICAL_ERR;
                } else if (ch == '\"') {
                   if (setTokenString(token, bufferPtr) == ALLOC_ERR) {
                       myStrFree(bufferPtr);
                       return INTERNAL_ERR;
                   }
                   return setTokenType(token, TOKEN_TYPE_STRING, bufferPtr);
                } else if (ch == '\\') {
                    state = SCANNER_STATE_ESCAPE_SEQUENCE;
                } else {
                    if (myStrApp(bufferPtr, ch) == ALLOC_ERR) {
                        myStrFree(bufferPtr);
                        return INTERNAL_ERR;
                    }
                }        
            break;

            case SCANNER_STATE_ESCAPE_SEQUENCE:
                if (ch == '\\') {
                    if (myStrApp(bufferPtr, '\\') == ALLOC_ERR) {
                        myStrFree(bufferPtr);
                        return INTERNAL_ERR;
                    }
                    state = SCANNER_STATE_STRING;
                } else if (ch == 'n') {
                    if (myStrApp(bufferPtr, 10) == ALLOC_ERR) {
                        myStrFree(bufferPtr);
                        return INTERNAL_ERR;
                    }
                    state = SCANNER_STATE_STRING;
                } else if (ch == 't') {
                    if (myStrApp(bufferPtr, 9) == ALLOC_ERR) {
                        myStrFree(bufferPtr);
                        return INTERNAL_ERR;
                    }
                    state = SCANNER_STATE_STRING;
                } else if (ch == '\"') {
                    if (myStrApp(bufferPtr, '\"') == ALLOC_ERR) {
                        myStrFree(bufferPtr);
                        return INTERNAL_ERR;
                    }
                    state = SCANNER_STATE_STRING;
                } else if (ch == 'x') {
                    state = SCANNER_STATE_HEXAONE;
                } else {
                    ///invalid escape sequence
                    myStrFree(bufferPtr);
                    return LEXICAL_ERR;
                }
            break;

            case SCANNER_STATE_HEXAONE:

                if  (isdigit(ch) || (tolower(ch) >= 'a' && tolower(ch) <= 'f'))   {
                    ///buffering first hexa digit
                    hexbuff[0] = ch;
                    state = SCANNER_STATE_HEXATWO;
                } else {
                    myStrFree(bufferPtr);
                    return LEXICAL_ERR;
                }
            break;

            case SCANNER_STATE_HEXATWO:
                if  (isdigit(ch) || (tolower(ch) >= 'a' && tolower(ch) <= 'f')) { 
                    ///append converted hex value to the string constant
                    hexbuff[1] = ch;
                    unsigned val = (int) strtol(hexbuff, NULL, 16);
                    if (myStrApp(bufferPtr, val) == ALLOC_ERR) {
                        myStrFree(bufferPtr);
                        return INTERNAL_ERR;
                    }
                    state = SCANNER_STATE_STRING;
                } else {
                    myStrFree(bufferPtr);
                    return LEXICAL_ERR;
                }
            break;

            case SCANNER_STATE_EOL:
                if (isspace(ch) || (int) ch == EOL) {
                    break;
                }
                ungetc(ch, file);
                return setTokenType(token, TOKEN_TYPE_EOL, bufferPtr);
            break;
        }
    }
}