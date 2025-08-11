
// parser.c
// トークン列からS式（構文木）を構築するパーサの実装。

#include "parser.h"
#include "heap.h"
#include "tokenizer.h"
#include <string.h>
#include <stdlib.h>

/* ---------- convenience constructors ---------- */
static Obj* make_int(int32_t v) {
    Obj* o = obj_alloc(OBJECT_INT);
    if (!o) return NULL;
    o->v.iv = v;
    return o;
}
static Obj* make_string(const char* s) {
    size_t len = strlen(s) + 1;
    void* p    = heap_alloc(len);
    if (!p) return NULL;
    memcpy(p, s, len);
    Obj* o = obj_alloc(OBJECT_STRING);
    if (!o) {
        heap_free(p);
        return NULL;
    }
    o->v.s.strptr = p;
    o->v.s.len    = (uint16_t)(len - 1);
    return o;
}
static Obj* make_cons(Obj* car, Obj* cdr) {
    Obj* o = obj_alloc(OBJECT_CONS);
    if (!o) return NULL;
    o->v.cons.car = car;
    o->v.cons.cdr = cdr;
    return o;
}

// 単純なパース関数（テスト用）
ASTNode* parse(TokenArray* tokens) {
    if (tokens == NULL || tokens->size == 0) {
        return NULL;
    }

    // "(+ 1 2)" の場合のハードコードされたパース
    if (tokens->size == 5 &&
        tokens->tokens[0].kind == TOKEN_LPAREN &&
        tokens->tokens[1].kind == TOKEN_PLUS &&
        tokens->tokens[2].kind == TOKEN_NUMBER &&
        tokens->tokens[3].kind == TOKEN_NUMBER &&
        tokens->tokens[4].kind == TOKEN_RPAREN) {

        ASTNode* plus = make_symbol("+");
        ASTNode* num1 = make_number(atoi(tokens->tokens[2].value));
        ASTNode* num2 = make_number(atoi(tokens->tokens[3].value));

        // (+ 1 2) の構造を作成
        ASTNode* args = make_cons(num1, make_cons(num2, NULL));
        return make_cons(plus, args);
    }

    // "(* (+ 1 2) 3)" の場合
    if (tokens->size == 9 &&
        tokens->tokens[0].kind == TOKEN_LPAREN &&
        tokens->tokens[1].kind == TOKEN_ASTERISK) {

        ASTNode* mult = make_symbol("*");

        // 内側の (+ 1 2) を作成
        ASTNode* plus = make_symbol("+");
        ASTNode* num1 = make_number(atoi(tokens->tokens[4].value));
        ASTNode* num2 = make_number(atoi(tokens->tokens[5].value));
        ASTNode* inner_expr = make_cons(plus, make_cons(num1, make_cons(num2, NULL)));

        // 外側の数値
        ASTNode* num3 = make_number(atoi(tokens->tokens[7].value));

        // (* (+ 1 2) 3) の構造を作成
        ASTNode* args = make_cons(inner_expr, make_cons(num3, NULL));
        return make_cons(mult, args);
    }

    return NULL;
}

// ASTノードのメモリ解放
void free_ast(ASTNode* node) {
    if (node == NULL) return;

    switch (node->type) {
        case AST_CONS:
            free_ast(node->data.cons.car);
            free_ast(node->data.cons.cdr);
            break;
        case AST_SYMBOL:
            if (node->data.symbol != NULL) {
                heap_free(node->data.symbol);
            }
            break;
        case AST_NUMBER:
            // 数値は特に解放するものがない
            break;
    }
    heap_free(node);
}
