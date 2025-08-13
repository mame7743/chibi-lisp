#include "object.h"
#include "object_pool.h"
#include "gc.h"
#include "heap.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//------------------------------------------
// 固定オブジェクト（object_pool外で管理）
//------------------------------------------
static Object fixed_nil   = {.type = OBJ_NIL};
static Object fixed_true  = {.type = OBJ_BOOL, .data.number = 1};
static Object fixed_false = {.type = OBJ_BOOL, .data.number = 0};

// グローバルポインタ
Object* obj_nil   = &fixed_nil;
Object* obj_true  = &fixed_true;
Object* obj_false = &fixed_false;


// 内部定数（このファイルでのみ使用）
static const char* const NIL_STRING = "nil";
static const char* const TRUE_STRING = "t";
static const char* const FALSE_STRING = "nil";
static const char* const FUNCTION_PREFIX = "#<function>";
static const char* const LAMBDA_PREFIX = "#<lambda>";

//------------------------------------------
// オブジェクトシステム初期化
//------------------------------------------
void object_system_init(void) {
    // ヒープ、プール、GCを初期化
    heap_init();
    object_pool_init();
    gc_init();
}

void object_system_cleanup(void) {
    // 固定オブジェクトはstatic領域なので特に何もしない
    // obj_nil, obj_true, obj_falseはstatic領域にあるため、
    // グローバルポインタをNULLにする必要はない
}

//------------------------------------------
// オブジェクト作成関数
//------------------------------------------
Object* make_number(int value) {
    Object* obj = object_pool_alloc();
    if (!obj) return NULL;
    memset(obj, 0, sizeof(Object));  // 全体をゼロクリア
    obj->type        = OBJ_NUMBER;
    obj->data.number = value;
    return obj;
}

Object* make_string(const char* text) {
    Object* obj = object_pool_alloc();
    if (!obj) return NULL;
    memset(obj, 0, sizeof(Object));  // 全体をゼロクリア
    obj->type               = OBJ_STRING;
    obj->data.string.length = strlen(text);
    obj->data.string.text   = heap_alloc(obj->data.string.length + 1);
    if (!obj->data.string.text) {
        object_pool_free(obj);
        return NULL;
    }
    strcpy(obj->data.string.text, text);
    return obj;
}

Object* make_symbol(const char* name) {
    Object* obj = object_pool_alloc();
    if (!obj) return NULL;
    memset(obj, 0, sizeof(Object));  // 全体をゼロクリア
    obj->type              = OBJ_SYMBOL;
    obj->data.symbol.length = strlen(name);
    obj->data.symbol.name  = heap_alloc(obj->data.symbol.length + 1);
    if (!obj->data.symbol.name) {
        object_pool_free(obj);
        return NULL;
    }
    strcpy(obj->data.symbol.name, name);
    return obj;
}

Object* make_cons(Object* car, Object* cdr) {
    Object* obj = object_pool_alloc();
    if (!obj) return NULL;
    memset(obj, 0, sizeof(Object));  // 全体をゼロクリア
    obj->type          = OBJ_CONS;
    obj->data.cons.car = car;
    obj->data.cons.cdr = cdr;
    return obj;
}

Object* make_function(Object* (*func)(Object*)) {
    Object* obj = object_pool_alloc();
    if (!obj) return NULL;
    memset(obj, 0, sizeof(Object));  // 全体をゼロクリア
    obj->type                      = OBJ_FUNCTION;
    obj->data.function.native_func = func;
    obj->data.function.params      = NULL;
    obj->data.function.body        = NULL;
    return obj;
}

Object* make_lambda(Object* params, Object* body) {
    Object* obj = object_pool_alloc();
    if (!obj) return NULL;
    memset(obj, 0, sizeof(Object));  // 全体をゼロクリア
    obj->type                   = OBJ_LAMBDA;
    obj->data.function.params   = params;
    obj->data.function.body     = body;
    obj->data.function.native_func = NULL;
    return obj;
}

//------------------------------------------
// 型チェック関数
//------------------------------------------
bool is_nil(Object* obj) { return obj == obj_nil; }
bool is_number(Object* obj) { return obj && obj->type == OBJ_NUMBER; }
bool is_string(Object* obj) { return obj && obj->type == OBJ_STRING; }
bool is_symbol(Object* obj) { return obj && obj->type == OBJ_SYMBOL; }
bool is_cons(Object* obj) { return obj && obj->type == OBJ_CONS; }
bool is_function(Object* obj) { return obj && obj->type == OBJ_FUNCTION; }
bool is_lambda(Object* obj) { return obj && obj->type == OBJ_LAMBDA; }

//------------------------------------------
// アクセサ関数
//------------------------------------------
int obj_number_value(Object* obj) {
    return is_number(obj) ? obj->data.number : 0;
}

const char* obj_string_text(Object* obj) {
    return is_string(obj) ? obj->data.string.text : "";
}

const char* obj_symbol_name(Object* obj) {
    return is_symbol(obj) ? obj->data.symbol.name : "";
}

Object* obj_car(Object* obj) {
    return is_cons(obj) ? obj->data.cons.car : obj_nil;
}

Object* obj_cdr(Object* obj) {
    return is_cons(obj) ? obj->data.cons.cdr : obj_nil;
}

//------------------------------------------
// リスト操作
//------------------------------------------
Object* obj_list_length(Object* list) {
    int count = 0;
    while (is_cons(list)) {
        count++;
        list = obj_cdr(list);
    }
    return make_number(count);
}

Object* obj_list_nth(Object* list, int n) {
    while (n > 0 && is_cons(list)) {
        list = obj_cdr(list);
        n--;
    }
    return is_cons(list) ? obj_car(list) : obj_nil;
}

Object* obj_list_append(Object* list1, Object* list2) {
    if (!is_cons(list1)) return list2;

    Object* result = make_cons(obj_car(list1), obj_list_append(obj_cdr(list1), list2));
    return result;
}

//------------------------------------------
// デバッグ用関数
//------------------------------------------
void object_dump(Object* obj) {
    if (!obj) {
        printf("NULL");
        return;
    }

    switch (obj->type) {
        case OBJ_NIL:
            printf(NIL_STRING);
            break;
        case OBJ_BOOL:
            printf(obj->data.number ? TRUE_STRING : NIL_STRING);
            break;
        case OBJ_NUMBER:
            printf("%d", obj->data.number);
            break;
        case OBJ_STRING:
            printf("\"%s\"", obj->data.string.text);
            break;
        case OBJ_SYMBOL:
            printf("%s", obj->data.symbol.name);
            break;
        case OBJ_CONS:
            printf("(");
            object_dump(obj->data.cons.car);
            printf(" . ");
            object_dump(obj->data.cons.cdr);
            printf(")");
            break;
        case OBJ_FUNCTION:
            printf(FUNCTION_PREFIX);
            break;
        case OBJ_LAMBDA:
            printf(LAMBDA_PREFIX);
            break;
    }
}