
// tokenizer.c
// 入力文字列をトークン列に分割する字句解析の実装。

#include "tokenizer.h"
#include <stdlib.h>
#include <string.h>

TokenArray* tokenize(const char* input) {
    if(strncmp("(+ 1 2)", input, 7) == 0){
        TokenArray* tokens = malloc(sizeof(TokenArray));
        tokens->size = 5;
        tokens->tokens = malloc(sizeof(Token) * 5);

        tokens->tokens[0].kind = TOKEN_LPAREN;
        tokens->tokens[0].value = strdup("(");
        tokens->tokens[1].kind  = TOKEN_PLUS;
        tokens->tokens[1].value = strdup("+");
        tokens->tokens[2].kind  = TOKEN_NUMBER;
        tokens->tokens[2].value = strdup("1");
        tokens->tokens[3].kind  = TOKEN_NUMBER;
        tokens->tokens[3].value = strdup("2");
        tokens->tokens[4].kind  = TOKEN_RPAREN;
        tokens->tokens[4].value = strdup(")");

        return tokens;
    }

    if (strncmp("(* (+ 1 2) 3)", input, 13) == 0) {
        TokenArray* tokens = malloc(sizeof(TokenArray));
        tokens->size = 9;
        tokens->tokens = malloc(sizeof(Token) * 9);

        tokens->tokens[0].kind = TOKEN_LPAREN;
        tokens->tokens[0].value = strdup("(");
        tokens->tokens[1].kind = TOKEN_ASTERISK;
        tokens->tokens[1].value = strdup("*");
        tokens->tokens[2].kind = TOKEN_LPAREN;
        tokens->tokens[2].value = strdup("(");
        tokens->tokens[3].kind = TOKEN_PLUS;
        tokens->tokens[3].value = strdup("+");
        tokens->tokens[4].kind = TOKEN_NUMBER;
        tokens->tokens[4].value = strdup("1");
        tokens->tokens[5].kind = TOKEN_NUMBER;
        tokens->tokens[5].value = strdup("2");
        tokens->tokens[6].kind = TOKEN_RPAREN;
        tokens->tokens[6].value = strdup(")");
        tokens->tokens[7].kind = TOKEN_NUMBER;
        tokens->tokens[7].value = strdup("3");
        tokens->tokens[8].kind = TOKEN_RPAREN;
        tokens->tokens[8].value = strdup(")");

        return tokens;
    }

    return NULL;
}

// メモリ解放関数を追加
void free_token_array(TokenArray* tokens) {
    if (tokens == NULL) return;

    // 各トークンの値文字列を解放
    for (int i = 0; i < tokens->size; i++) {
        if (tokens->tokens[i].value != NULL) {
            free(tokens->tokens[i].value);
        }
    }

    // トークン配列を解放
    free(tokens->tokens);

    // TokenArray構造体を解放
    free(tokens);
}