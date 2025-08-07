#ifndef __TOKEN_H__
#define __TOKEN_H__

#include <stdint.h>
#include <stddef.h>

typedef enum {
    TOKEN_TYPE_NONE = 0,
    TOKEN_TYPE_INTEGER,
    TOKEN_TYPE_STRING,
    TOKEN_TYPE_FLOAT,
    TOKEN_TYPE_BOOLEAN,
    TOKEN_TYPE_CUSTOM
} TokenType;

typedef struct {
    TokenType type;  // Type of the token
    size_t length; // Length of the token data
    char *literal; // Pointer to the token data
    Token *next; // Pointer to the next token in the list
} Token;

#endif // __TOKEN_H__