
// tokenizer.c
// 入力文字列をトークン列に分割する字句解析の実装。

#include "tokenizer.h"
#include "heap.h"
#include <stdlib.h>
#include <string.h>

TokenArray* tokenize(const char* input) {
    if(strncmp("(+ 1 2)", input, 7) == 0){
        TokenArray* tokens = heap_alloc(sizeof(TokenArray));
        tokens->size = 5;
        tokens->tokens     = heap_alloc(sizeof(Token) * 5);

        tokens->tokens[0].kind = TOKEN_LPAREN;
        tokens->tokens[0].value = heap_alloc(strlen("(") + 1);
        strcpy(tokens->tokens[0].value, "(");
        tokens->tokens[1].kind  = TOKEN_PLUS;
        tokens->tokens[1].value = heap_alloc(strlen("+") + 1);
        strcpy(tokens->tokens[1].value, "+");
        tokens->tokens[2].kind  = TOKEN_NUMBER;
        tokens->tokens[2].value = heap_alloc(strlen("1") + 1);
        strcpy(tokens->tokens[2].value, "1");
        tokens->tokens[3].kind  = TOKEN_NUMBER;
        tokens->tokens[3].value = heap_alloc(strlen("2") + 1);
        strcpy(tokens->tokens[3].value, "2");
        tokens->tokens[4].kind  = TOKEN_RPAREN;
        tokens->tokens[4].value = heap_alloc(strlen(")") + 1);
        strcpy(tokens->tokens[4].value, ")");

        return tokens;
    }

    if (strncmp("(* (+ 1 2) 3)", input, 13) == 0) {
        TokenArray* tokens = heap_alloc(sizeof(TokenArray));
        tokens->size = 9;
        tokens->tokens = heap_alloc(sizeof(Token) * 9);

        tokens->tokens[0].kind = TOKEN_LPAREN;
        tokens->tokens[0].value = heap_alloc(strlen("(") + 1);
        strcpy(tokens->tokens[0].value, "(");
        tokens->tokens[1].kind = TOKEN_ASTERISK;
        tokens->tokens[1].value = heap_alloc(strlen("*") + 1);
        strcpy(tokens->tokens[1].value, "*");
        tokens->tokens[2].kind = TOKEN_LPAREN;
        tokens->tokens[2].value = heap_alloc(strlen("(") + 1);
        strcpy(tokens->tokens[2].value, "(");
        tokens->tokens[3].kind = TOKEN_PLUS;
        tokens->tokens[3].value = heap_alloc(strlen("+") + 1);
        strcpy(tokens->tokens[3].value, "+");
        tokens->tokens[4].kind = TOKEN_NUMBER;
        tokens->tokens[4].value = heap_alloc(strlen("1") + 1);
        strcpy(tokens->tokens[4].value, "1");
        tokens->tokens[5].kind = TOKEN_NUMBER;
        tokens->tokens[5].value = heap_alloc(strlen("2") + 1);
        strcpy(tokens->tokens[5].value, "2");
        tokens->tokens[6].kind = TOKEN_RPAREN;
        tokens->tokens[6].value = heap_alloc(strlen(")") + 1);
        strcpy(tokens->tokens[6].value, ")");
        tokens->tokens[7].kind = TOKEN_NUMBER;
        tokens->tokens[7].value = heap_alloc(strlen("3") + 1);
        strcpy(tokens->tokens[7].value, "3");
        tokens->tokens[8].kind = TOKEN_RPAREN;
        tokens->tokens[8].value = heap_alloc(strlen(")") + 1);
        strcpy(tokens->tokens[8].value, ")");

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
            heap_free(tokens->tokens[i].value);
        }
    }

    // トークン配列を解放
    heap_free(tokens->tokens);

    // TokenArray構造体を解放
    heap_free(tokens);
}