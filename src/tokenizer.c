
// tokenizer.c
// 入力文字列をトークン列に分割する字句解析の実装。

#include "token.h"
#include <stdlib.h>
#include <string.h>

// トークンを初期化する関数
Token *new_token(TokenType type, const char *literal) {
    Token *token = (Token *)calloc(1,sizeof(Token));
    if (!token) {
        return NULL; // メモリ割り当て失敗
    }

    token->type = type;
    token->length = strlen(literal);
    token->literal = (char *)calloc(1,token->length + 1); // +1 for null terminator
    if (!token->literal) {
        free(literal);
        return NULL; // メモリ割り当て失敗
    }
    token->next = NULL;

    // リテラルをコピー
    strcpy(token->literal, literal);
    return token;
}

// トークンを解放する関数
void free_token(Token *token) {
    if (token) {
        free(token->literal);
        free(token);
    }
}

// トークンをリストに追加する関数
Token* add_token(Token *head, Token *new_token) {
    if (!head) {
        return new_token; // リストが空なら新しいトークンを返す
    }

    Token *current = head;
    while (current->next) {
        current = current->next; // リストの最後まで移動
    }
    current->next = new_token; // 新しいトークンをリストの最後に追加
    return head; // 変更されたリストの先頭を返す
}

Token* tokenize(const char *input) {
    Token *head = NULL;
    Token *current = NULL;

    Token head = {};
    Token *cur = &head;

    char str[256];
    char *p = input;

    while (*p) {

        // Skip whitespace
        if(isspace(*p)){
            p++;
            continue;
        }

        // Numeric token
        if (isdigit(*p)) {
            char *q = p++;
            for (;;){
                if (!isdigit(*p)) {
                    break; // 数字以外の文字が来たら終了
                }
                p++;
            }
            strncpy(str, q, p - q);
            tok = new_token(TOKEN_TYPE_INTEGER, str);
        }
    }

    free(input_copy); // コピーした入力文字列を解放
    return head; // トークンリストの先頭を返す
}