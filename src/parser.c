// parser.c
// トークン列からS式（構文木）を構築するパーサの実装。

#include "parser.h"
#include "object.h"
#include "object_pool.h"
#include "tokenizer.h"
#include <string.h>
#include <stdlib.h>
// 必要な構造体や定数の宣言
#define DEPTH_MAX 256

Object *parse(const char *src) {
    TokenArray *tokens = tokenize(src);
    if (!tokens) return obj_nil;

    size_t index = 0;
    Object *result = parse_expression(tokens, &index);

    free_token_array(tokens);
    return result ? result : obj_nil;
}

// 式をパースする再帰関数
Object *parse_expression(TokenArray *tokens, size_t *index) {
    if (*index >= tokens->size) {
        return obj_nil;
    }

    Token token = tokens->tokens[*index];

    switch (token.kind) {
        case TOKEN_LPAREN: {
            // リストの開始
            (*index)++;
            Object *head = obj_nil;
            Object **current = &head;

            while (*index < tokens->size && tokens->tokens[*index].kind != TOKEN_RPAREN) {
                Object *elem = parse_expression(tokens, index);
                if (!elem) return obj_nil;

                *current = make_cons(elem, obj_nil);
                current = &((*current)->data.cons.cdr);
            }

            if (*index >= tokens->size || tokens->tokens[*index].kind != TOKEN_RPAREN) {
                // 対応する閉じ括弧がない
                return obj_nil;
            }
            (*index)++; // 閉じ括弧を消費
            return head;
        }

        case TOKEN_SYMBOL: {
            (*index)++;
            return make_symbol(token.value);
        }

        case TOKEN_NUMBER: {
            (*index)++;
            return make_number(atoi(token.value));
        }

        case TOKEN_PLUS: {
            (*index)++;
            return make_symbol("+");
        }

        case TOKEN_MINUS: {
            (*index)++;
            return make_symbol("-");
        }

        case TOKEN_ASTERISK: {
            (*index)++;
            return make_symbol("*");
        }

        case TOKEN_SLASH: {
            (*index)++;
            return make_symbol("/");
        }

        default:
            // 他のトークンは無視
            (*index)++;
            return obj_nil;
    }
}
