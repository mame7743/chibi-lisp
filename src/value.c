// value.c
// Lispデータ型（Value）の実装。

#include "value.h"
#include "lisp_heap.h"
#include <stdlib.h>
#include <string.h>

// 固定値の定義
static const Value __nil = { VALUE_NIL, { .cons = { NULL, NULL } } };
static const Value __true_value = { VALUE_BOOL, {.number = 1} };
static const Value __false_value = { VALUE_BOOL, {.number = 0} };

const Value* nil = &__nil;
const Value* true_value = &__true_value;
const Value* false_value = &__false_value;

Value* make_int_value(int value) {
    Value* v = lisp_heap_alloc();
    if (v == NULL) return NULL;  // メモリ割り当て失敗
    v->kind = VALUE_INT;
    v->data.number = value;
    return v;
}

Value* make_string_value(const char* str) {
    Value* v = lisp_heap_alloc();
    if (v == NULL) return NULL;  // メモリ割り当て失敗
    v->kind = VALUE_STRING;
    // 文字列は固定バッファサイズに制限するか、別途管理する
    // 簡単のため、文字列ポインタをそのまま保存（コピーしない）
    v->data.string.text = malloc(strlen(str) + 1);
    if (v->data.string.text == NULL) {
        lisp_heap_free(v);
        return NULL;  // メモリ割り当て失敗
    }
    strcpy(v->data.string.text, str);
    v->data.string.len = strlen(str);
    return v;
}

Value* make_cons_value(Value* car, Value* cdr) {
    Value* v = lisp_heap_alloc();
    if (v == NULL) return NULL;  // メモリ割り当て失敗
    v->kind = VALUE_CONS;
    v->data.cons.car = car;
    v->data.cons.cdr = cdr;
    return v;
}

Value* make_symbol_value(const char* symbol) {
    Value* v = lisp_heap_alloc();
    if (v == NULL) return NULL;  // メモリ割り当て失敗
    v->kind = VALUE_SYMBOL;
    // シンボルも文字列のコピーを作成
    char* symbol_copy = malloc(strlen(symbol) + 1);
    if (symbol_copy == NULL) {
        lisp_heap_free(v);
        return NULL;  // メモリ割り当て失敗
    }
    strcpy(symbol_copy, symbol);
    v->data.symbol = symbol_copy;
    return v;
}
