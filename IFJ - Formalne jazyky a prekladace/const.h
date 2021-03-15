/*  const.h
*   All constants used in project
*/

// Maximum symtable size, must be a prime
#define TABLE_MAX_SIZE  509

// AST Default malloc sizes
#define PROG_BODY_DEF_SIZE 5
#define FUNC_PARAMS_DEF_SIZE 4
#define FUNC_RETTYPES_DEF_SIZE 4
#define FUNCCALL_ARGS_DEF_SIZE 4
#define FUNC_BODY_DEF_SIZE 4
#define ASSIGN_IDS_SIZE 4
#define ASSIGN_EXPR_SIZE 4


#define STATUS_OK       0x20
#define STATUS_ERR      0x21
#define STATUS_FINISH   0x22

#define SCANNER_ERR     0x200

#define EOL             0xA