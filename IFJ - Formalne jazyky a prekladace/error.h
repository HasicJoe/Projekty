/**
 * @file error.h
 * @author Miroslav Štěpánek (xstepa68@stud.fit.vutbr.cz)
 * @brief Constants for error and success states
 * @date 2020-12-03
 * 
 */

#define SUCCESS 0                   // no errors found
#define LEXICAL_ERR 1               // wrong structure of current lexeme
#define SYNTACTIC_ERR 2             // wrong program syntax, unexpected EOL
#define SEMANTIC_DEFINE_ERR 3       // undefined function/variable, redefinition of function/variable, etc.
#define SEMANTIC_DATATYPE_ERR 4     // getting data type of newly defined variable
#define SEMANTIC_TYPECOMPAT_ERR 5   // type compatability in arithmetic, string and relational expressions
#define SEMANTIC_PARAM_OR_RET_ERR 6 // wrong number or type of parameters/returned values when calling function/returning from function
#define SEMANTIC_OTHER_ERR 7        // other semantic errors
#define SEMANTIC_DIVZERO_ERR 9      // division by zero constant
#define INTERNAL_ERR 99             // error independent of the input program - memory allocation, etc.
