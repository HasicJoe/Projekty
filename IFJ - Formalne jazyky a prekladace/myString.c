/**
 * @file myString.c
 * @author Daniel Gavenda (xgaven08@stud.fit.vutbr.cz)
 * @brief Dynamic string
 * @date 2020-10-10
 * 
 */

#include "myString.h"

int myStrInit(MyString *str) {
    str->data = (char *) malloc(INIT_ALLOC_SIZE);

    if (str->data == NULL) {
        return ALLOC_ERR;
    }
    str->length = 0;
    str->data[str->length] = '\0';
    str->allocLength = INIT_ALLOC_SIZE;

    return NO_ERR;
}

void myStrFree(MyString *str) {
    free(str->data);
}

void myStrClear(MyString *str) {
    str->length = 0;
    str->data[str->length] = '\0';
}

int myStrApp(MyString *str, char ch) {
    if (str->length + 2 > str->allocLength) {
        unsigned newLength = str->length + INIT_ALLOC_SIZE;
        str->data = (char *) realloc(str->data, newLength);
        if (str->data == NULL) {
            return ALLOC_ERR;
        }
        str->allocLength = newLength;
    }
    str->data[str->length++] = ch;
    str->data[str->length] = '\0';

    return NO_ERR;
}

int myStrCat(MyString *str1, const char *str2) {
    unsigned str2len = strlen(str2);

    if (str1->length + str2len + 1 > str1->allocLength) {
        unsigned newLength = str1->length + str2len + 1;
        str1->data = (char *) realloc(str1->data, newLength);

        if (str1->data == NULL) {
            return ALLOC_ERR;
        }
    }
    //copy data
    for (unsigned i = str1->length; i < str1->length+str2len; i++) {
        str1->data[i] = str2[i-str1->length];
    }
    
    str1->length += str2len;
    str1->data[str1->length] = '\0';

    return NO_ERR;
}

int myStrCmp(MyString *str1, const char *str2) {
    return strcmp(str1->data, str2);
}

int myStrCopy(MyString *src, char *dest) {
    dest = (char*) malloc(strlen(src->data) + 1);
    if (dest == NULL) {
        return ALLOC_ERR;
    }
    strcpy(dest, src->data);
    return NO_ERR;
}
