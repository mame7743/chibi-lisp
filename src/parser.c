// parser.c
// トークン列からS式（構文木）を構築するパーサの実装。

#include "parser.h"
#include "object.h"
#include "object_pool.h"
#include "tokenizer.h"
#include <string.h>
#include <stdlib.h>
#define DEPTH_MAX 256

static Object* make_atom_token(const Token *t) {
    switch (t->kind) {
        case TOKEN_SYMBOL:   return make_symbol(t->value);
        case TOKEN_NUMBER:   return make_number(atoi(t->value));
        case TOKEN_STRING:   return make_string(t->value);
        case TOKEN_NIL:      return obj_nil;
        case TOKEN_TRUE:     return obj_true;
        case TOKEN_FALSE:    return obj_nil;  // falseはnilと同じ
        case TOKEN_PLUS:     return obj_plus;
        case TOKEN_MINUS:    return obj_minus;
        case TOKEN_ASTERISK: return obj_asterisk;
        case TOKEN_SLASH:    return obj_slash;
        case TOKEN_EQ:       return obj_eq;
        case TOKEN_LT:       return obj_lt;
        case TOKEN_GT:       return obj_gt;
        case TOKEN_LTE:      return obj_lte;
        case TOKEN_GTE:      return obj_gte;
        // 組み込み関数
        case TOKEN_PRINT:    return obj_print;
        case TOKEN_PRINTLN:  return obj_println;
        case TOKEN_STR:      return obj_str;
        case TOKEN_LENGTH:   return obj_length;
        case TOKEN_BOOLP:    return obj_boolp;
        // タイマー関数
        case TOKEN_NOW:      return obj_now;
        case TOKEN_SLEEP:    return obj_sleep;
        case TOKEN_TIME_DIFF: return obj_time_diff;
        // ループ制御関数
        case TOKEN_DOTIMES:   return obj_dotimes;
        default:             return obj_nil;
    }
}

// 汎用: index 位置から1式パースし index 更新
Object *parse_expression(TokenArray *tokens, size_t *index) {
    if (!tokens || !index) return obj_nil;
    size_t i = *index;
    if (i >= tokens->size) return obj_nil;

    typedef struct { Object *head; Object **current; } Frame;
    Frame stack[DEPTH_MAX];
    int sp = -1;

    if (tokens->tokens[i].kind != TOKEN_LPAREN) {
        Object *atom = make_atom_token(&tokens->tokens[i]);
        *index = i + 1;
        return atom;
    }

    while (1) {
        if (i >= tokens->size) {
            if (sp >= 0) { *index = i; return stack[0].head ? stack[0].head : obj_nil; }
            *index = i; return obj_nil;
        }
        Token *tok = &tokens->tokens[i];
        if (tok->kind == TOKEN_LPAREN) {
            if (sp + 1 >= DEPTH_MAX) { *index = i; return obj_nil; }
            ++sp; stack[sp].head = obj_nil; stack[sp].current = &stack[sp].head; i++; continue;
        } else if (tok->kind == TOKEN_RPAREN) {
            i++;
            Object *completed = obj_nil;
            if (sp >= 0) { completed = stack[sp].head; --sp; } else { *index = i; return obj_nil; }
            if (sp < 0) { *index = i; return completed ? completed : obj_nil; }
            *(stack[sp].current) = make_cons(completed, obj_nil);
            stack[sp].current = &((*(stack[sp].current))->data.cons.cdr);
            continue;
        } else {
            Object *atom = make_atom_token(tok); i++;
            if (sp < 0) { *index = i; return atom; }
            *(stack[sp].current) = make_cons(atom, obj_nil);
            stack[sp].current = &((*(stack[sp].current))->data.cons.cdr);
            continue;
        }
    }
}

// 最初の1式のみ互換API
Object *parse(const char *src) {
    TokenArray *tokens = tokenize(src);
    if (!tokens) return obj_nil;
    size_t idx = 0;
    Object *result = parse_expression(tokens, &idx);
    free_token_array(tokens);
    return result ? result : obj_nil;
}

// 複数トップレベル式を全て読みリストにする
Object *parse_all(const char *src) {
    TokenArray *tokens = tokenize(src);
    if (!tokens) return obj_nil;
    size_t idx = 0;
    Object *head = obj_nil; Object **cur = &head;
    while (idx < tokens->size) {
        // 空白や無効トークンをスキップする仕組みがなければ直接 parse_expression
        Object *expr = parse_expression(tokens, &idx);
        if (!expr) break; // エラー / 進展しない場合終了
        *cur = make_cons(expr, obj_nil);
        cur = &((*cur)->data.cons.cdr);
    }
    free_token_array(tokens);
    return head ? head : obj_nil;
}
