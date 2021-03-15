/**
 * @file myString.h
 * @author Daniel Gavenda (xgaven08@stud.fit.vutbr.cz)
 * @brief Dynamic string
 * @date 2020-10-10
 * 
 */

#ifndef myStringHeader
#define myStringHeader

#include <stdlib.h>
#include <string.h>

/**
 * Initial allocation length of an string
 */
#define INIT_ALLOC_SIZE 16

#define ALLOC_ERR 0x64
#define NO_ERR 0x0

typedef struct {
    char* data;
    unsigned int length;
    unsigned int allocLength;
} MyString;

/**
 * Initialization of dynamic string
 */ 
int myStrInit(MyString *str);

/**
 * 
 * Dealocate dynamic string 
 */
void myStrFree(MyString *str);
/**
 * 
 * Set data to an empty array
 */
void myStrClear(MyString *str);

/**
 * 
 * 
 * Append a char to data
 */
int myStrApp(MyString *str, char ch);

/**
 * 
 * Concatenate with a const char array
 * 
 */
int myStrCat(MyString *str1, const char *str2);

/**
 * 
 * 
 * Compare two dynamic strings
 */
int myStrCmp(MyString *str1, const char *str2);

/**
 * 
 * 
 * 
 * Copy string
 * 
 */
int myStrCopy(MyString *src, char *dest);

#endif

