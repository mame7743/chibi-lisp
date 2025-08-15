// tokenizer.c
// 入力文字列をトークン列に分割する字句解析の実装。

#include "tokenizer.h"
#include "chibi_lisp.h"
#include "heap.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define STR_EQUAL(src, dst) (strncmp((src), (dst), strlen(dst)) == 0 )

bool is_whitespace(char c) {
    return c == ' ' || c == '\n' || c == '\t';
}

bool is_digit(char c) {
    return c >= '0' && c <= '9';
}

bool is_symbol_char(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_' || c == '?' || c == '!' || c == '-' || c == '=' || c == '<' || c == '>';
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

    const char *ch = input;
    while (*ch) {
        // 空白をスキップ
        while (is_whitespace(*ch)) {
            ch++;
        }
        if (*ch == '\0') {
            break;  // 文字列の終端
        }

        if (*ch == ';') {
            // コメント行は無視
            while (*ch && *ch != '\n') {
                ch++;
            }
            continue;  // 次のトークンへ
        }

        // トークンの種類を判定
        switch (*ch) {
            case '(':
                token.kind = TOKEN_LPAREN;
                token.value = heap_alloc(2);  // '(' と '\0'
                if (token.value == NULL) goto ERROR;  // メモリ割り当て失敗
                strcpy(token.value, "(");
                ch++;
                break;
            case ')':
                token.kind = TOKEN_RPAREN;
                token.value = heap_alloc(2);  // ')' と '\0'
                if (token.value == NULL) goto ERROR;  // メモリ割り当て失敗
                token.value[0] = ')';
                token.value[1] = '\0';
                ch++;
                break;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                token.kind        = TOKEN_NUMBER;
                const char *start = ch;
                while (is_digit(*ch)) ch++;
                size_t length = ch - start;
                token.value   = heap_alloc(length + 1);  // 数値と '\0'
                if (token.value == NULL) goto ERROR;     // メモリ割り当て失敗
                strncpy(token.value, start, length);
                token.value[length] = '\0';
                break;
            case '+':
                token.kind = TOKEN_PLUS;
                token.value = heap_alloc(2);  // '+' と '\0'
                if (token.value == NULL) goto ERROR;  // メモリ割り当て失敗
                token.value[0] = '+';
                token.value[1] = '\0';
                ch++;
                break;
            case '-':
                token.kind = TOKEN_MINUS;
                token.value = heap_alloc(2);  // '-' と '\0'
                if (token.value == NULL) goto ERROR;  // メモリ割り当e失敗
                token.value[0] = '-';
                token.value[1] = '\0';
                ch++;
                break;
            case '*':
                token.kind = TOKEN_ASTERISK;
                token.value = heap_alloc(2);  // '*' と '\0'
                if (token.value == NULL) goto ERROR;  // メモリ割り当て失敗
                token.value[0] = '*';
                token.value[1] = '\0';
                ch++;
                break;
            case '/':
                token.kind = TOKEN_SLASH;
                token.value = heap_alloc(2);  // '/' と '\0'
                if (token.value == NULL) goto ERROR;  // メモリ割り当て失敗
                token.value[0] = '/';
                token.value[1] = '\0';
                ch++;
                break;
            case '"':
                // 文字列リテラル
                token.kind = TOKEN_STRING;
                {
                    const char *str_start = ++ch;  // 開始クォートをスキップ
                    while (*ch && *ch != '"') ch++;  // 終了クォートまで進む
                    if (*ch != '"') goto ERROR;  // 閉じクォートがない
                    size_t str_length = ch - str_start;
                    token.value = heap_alloc(str_length + 1);
                    if (token.value == NULL) goto ERROR;  // メモリ割り当て失敗
                    strncpy(token.value, str_start, str_length);
                    token.value[str_length] = '\0';
                    ch++;  // 終了クォートをスキップ
                }
                break;
            case '=':
                token.kind = TOKEN_EQ;
                token.value = heap_alloc(2);  // '=' と '\0'
                if (token.value == NULL) goto ERROR;  // メモリ割り当て失敗
                token.value[0] = '=';
                token.value[1] = '\0';
                ch++;
                break;
            case '>':
                if (*(ch + 1) == '=') {
                    token.kind = TOKEN_GTE;
                    token.value = heap_alloc(3);  // ">=" と '\0'
                    if (token.value == NULL) goto ERROR;  // メモリ割り当て失敗
                    token.value[0] = '>';
                    token.value[1] = '=';
                    token.value[2] = '\0';
                    ch += 2;  // ">=" をスキップ
                } else {
                    token.kind = TOKEN_GT;
                    token.value = heap_alloc(2);  // '>' と '\0'
                    if (token.value == NULL) goto ERROR;  // メモリ割り当て失敗
                    token.value[0] = '>';
                    token.value[1] = '\0';
                    ch++;
                }
                break;
            case '<':
                if (*(ch + 1) == '=') {
                    token.kind = TOKEN_LTE;
                    token.value = heap_alloc(3);  // "<=" と '\0'
                    if (token.value == NULL) goto ERROR;  // メモリ割り当   て失敗
                    token.value[0] = '<';
                    token.value[1] = '=';
                    token.value[2] = '\0';
                    ch += 2;  // "<=" をスキップ
                } else {
                    token.kind = TOKEN_LT;
                    token.value = heap_alloc(2);  // '<' と '\0'
                    if (token.value == NULL) goto ERROR;  // メモリ割り当て失敗
                    token.value[0] = '<';
                    token.value[1] = '\0';
                    ch++;
                }
                break;
            default:
                if (is_symbol_char(*ch)) {
                    // シンボルとして扱う
                    const char *start = ch;
                    while (is_symbol_char(*ch)) ch++;
                    size_t length = ch - start;
                    token.value = heap_alloc(length + 1);  // シンボルと '\0'
                    if (token.value == NULL) goto ERROR;  // メモリ割り当て失敗
                    strncpy(token.value, start, length);
                    token.value[length] = '\0';

                    // トークンの種類を判定
                    if (STR_EQUAL(token.value, "lambda")) {
                        token.kind = TOKEN_LAMBDA;
                        break;
                    }

                    if (STR_EQUAL(token.value, "if")) {
                        token.kind = TOKEN_IF;
                        break;
                    }

                    if (STR_EQUAL(token.value, "quote")) {
                        token.kind = TOKEN_QUOTE;
                        break;
                    }

                    if (STR_EQUAL(token.value, "nil")) {
                        token.kind = TOKEN_NIL;
                        break;
                    }

                    if (STR_EQUAL(token.value, "t")) {
                        token.kind = TOKEN_TRUE;
                        break;
                    }

                    if (STR_EQUAL(token.value, "define")) {
                        token.kind = TOKEN_DEFINE;
                        break;
                    }
                    if (STR_EQUAL(token.value, "set!")) {
                        token.kind = TOKEN_SET;
                        break;
                    }

                    if (STR_EQUAL(token.value, "loop")) {
                        token.kind = TOKEN_LOOP;
                        break;
                    }

                    if (STR_EQUAL(token.value, "dotimes")) {
                        token.kind = TOKEN_DOTIMES;
                        break;
                    }

                    if (STR_EQUAL(token.value, "print")) {
                        token.kind = TOKEN_PRINT;
                        break;
                    }

                    if (STR_EQUAL(token.value, "println")) {
                        token.kind = TOKEN_PRINTLN;
                        break;
                    }

                    if (STR_EQUAL(token.value, "str")) {
                        token.kind = TOKEN_STR;
                        break;
                    }

                    if (STR_EQUAL(token.value, "length")) {
                        token.kind = TOKEN_LENGTH;
                        break;
                    }

                    if (STR_EQUAL(token.value, "bool?")) {
                        token.kind = TOKEN_BOOLP;
                        break;
                    }

                    if (STR_EQUAL(token.value, "now")) {
                        token.kind = TOKEN_NOW;
                        break;
                    }

                    if (STR_EQUAL(token.value, "sleep")) {
                        token.kind = TOKEN_SLEEP;
                        break;
                    }

                    if (STR_EQUAL(token.value, "time-diff")) {
                        token.kind = TOKEN_TIME_DIFF;
                        break;
                    }

                    token.kind = TOKEN_SYMBOL;

                } else {
                        // 未知の文字はスキップ
                        ch++;
                        continue;
                }

                break;
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