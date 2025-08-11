
// tokenizer.c
// 入力文字列をトークン列に分割する字句解析の実装。

#include "tokenizer.h"
#include "heap.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

bool is_whitespace(char c) {
    return c == ' ' || c == '\n' || c == '\t';
}

bool is_digit(char c) {
    return c >= '0' && c <= '9';
}

bool is_symbol_char(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_';
}

TokenArray* tokenize(const char* input) {

    TokenArray *tokens = heap_alloc(sizeof(TokenArray));
    Token token;

    if (tokens == NULL) {
        return NULL;  // メモリ割り当て失敗
    }

    // トークンの初期化
    tokens->size = 0;
    tokens->tokens = heap_alloc(sizeof(Token) * 10);  // 初期サイズ
    if (tokens->tokens == NULL) {
        heap_free(tokens);
        return NULL;
    }

    const char *p = input;
    while (*p) {
        // 空白をスキップ
        while (is_whitespace(*p)) {
            p++;
        }
        if (*p == '\0') {
            break;  // 文字列の終端
        }

        // トークンの種類を判定
        if (*p == '(') {
            token.kind = TOKEN_LPAREN;
            token.value = heap_alloc(2);  // '(' と '\0'
            if (token.value == NULL) goto ERROR;  // メモリ割り当て失敗
            strcpy(token.value, "(");
            p++;
        }
        else if (*p == ')') {
            token.kind = TOKEN_RPAREN;
            token.value = heap_alloc(2);  // ')' と '\0'
            if (token.value == NULL) goto ERROR;  // メモリ割り当て失敗
            token.value[0] = ')';
            token.value[1] = '\0';
            p++;
        }
        else if (is_digit(*p)) {
            token.kind = TOKEN_NUMBER;
            const char *start = p;
            while (is_digit(*p)) p++;
            size_t length = p - start;
            token.value = heap_alloc(length + 1);  // 数値と '\0'
            if (token.value == NULL) goto ERROR;  // メモリ割り当て失敗
            strncpy(token.value, start, length);
            token.value[length] = '\0';
        }
        else if (*p == '+') {
            token.kind = TOKEN_PLUS;
            token.value = heap_alloc(2);  // '+' と '\0'
            if (token.value == NULL) goto ERROR;  // メモリ割り当て失敗
            token.value[0] = '+';
            token.value[1] = '\0';
            p++;
        }
        else if (*p == '*') {
            token.kind = TOKEN_ASTERISK;
            token.value = heap_alloc(2);  // '*' と '\0'
            if (token.value == NULL) goto ERROR;  // メモリ割り当て失敗
            token.value[0] = '*';
            token.value[1] = '\0';
            p++;
        }
        else if (is_symbol_char(*p)) {
            // シンボルとして扱う
            token.kind = TOKEN_SYMBOL;
            const char *start = p;
            while (is_symbol_char(*p)) p++;
            size_t length = p - start;
            token.value = heap_alloc(length + 1);  // シンボルと '\0'
            if (token.value == NULL) goto ERROR;  // メモリ割り当て失敗
            strncpy(token.value, start, length);
            token.value[length] = '\0';
        }
        else {
            // 未知の文字はスキップ
            p++;
            continue;
        }

        // トークンを配列に追加
        tokens->tokens[tokens->size] = token;
        tokens->size++;
    }

    return tokens;

ERROR:
    heap_free(tokens->tokens);
    heap_free(tokens);
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