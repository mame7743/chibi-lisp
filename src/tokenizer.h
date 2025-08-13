#ifndef __TOKENIZER_H__
#define __TOKENIZER_H__

#include <stdint.h>
#include <stddef.h>

typedef enum {
    TOKEN_LPAREN = 0,  // (
    TOKEN_RPAREN,      // )
    TOKEN_SYMBOL,      // シンボル
    TOKEN_NUMBER,      // 数値
    TOKEN_PLUS,        // +
    TOKEN_MINUS,       // -
    TOKEN_ASTERISK,    // *
    TOKEN_SLASH,       // /
} TokenKind;

typedef struct {
    TokenKind kind;  // Type of the token
    char* value;     // セミコロンを追加
} Token;

typedef struct {
    size_t size;
    Token* tokens;
} TokenArray;

// 関数宣言
TokenArray* tokenize(const char* input);
void free_token_array(TokenArray* tokens);

#endif  // __TOKENIZER_H__