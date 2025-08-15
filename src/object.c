// object.c - Core LISP object system implementation
#include "chibi_lisp.h"
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
static const Object fixed_nil   = {.type = OBJ_NIL};
static const Object fixed_true  = {.type = OBJ_BOOL, .data.number = 1};
static const Object fixed_false = {.type = OBJ_BOOL, .data.number = 0};
static const Object fixed_void  = {.type = OBJ_VOID};

// 演算子シンボル定数オブジェクト
static const char plus_name[]     = "+";
static const char minus_name[]    = "-";
static const char asterisk_name[] = "*";
static const char slash_name[]    = "/";
static const char eq_name[]       = "=";
static const char lt_name[]       = "<";
static const char gt_name[]       = ">";
static const char lte_name[]      = "<=";
static const char gte_name[]      = ">=";

// 演算子定数オブジェクト
static const Object fixed_plus     = {.type = OBJ_OPERATOR, .data.operator_type = OP_PLUS};
static const Object fixed_minus    = {.type = OBJ_OPERATOR, .data.operator_type = OP_MINUS};
static const Object fixed_asterisk = {.type = OBJ_OPERATOR, .data.operator_type = OP_ASTERISK};
static const Object fixed_slash    = {.type = OBJ_OPERATOR, .data.operator_type = OP_SLASH};
static const Object fixed_eq       = {.type = OBJ_OPERATOR, .data.operator_type = OP_EQ};
static const Object fixed_lt       = {.type = OBJ_OPERATOR, .data.operator_type = OP_LT};
static const Object fixed_gt       = {.type = OBJ_OPERATOR, .data.operator_type = OP_GT};
static const Object fixed_lte      = {.type = OBJ_OPERATOR, .data.operator_type = OP_LTE};
static const Object fixed_gte      = {.type = OBJ_OPERATOR, .data.operator_type = OP_GTE};

// 組み込み関数定数オブジェクト
static const Object fixed_print    = {.type = OBJ_BUILTIN, .data.builtin_type = BUILTIN_PRINT};
static const Object fixed_println  = {.type = OBJ_BUILTIN, .data.builtin_type = BUILTIN_PRINTLN};
static const Object fixed_str      = {.type = OBJ_BUILTIN, .data.builtin_type = BUILTIN_STR};
static const Object fixed_length   = {.type = OBJ_BUILTIN, .data.builtin_type = BUILTIN_LENGTH};
static const Object fixed_boolp    = {.type = OBJ_BUILTIN, .data.builtin_type = BUILTIN_BOOLP};

// タイマー関数定数オブジェクト
static const Object fixed_now      = {.type = OBJ_BUILTIN, .data.builtin_type = BUILTIN_NOW};
static const Object fixed_sleep    = {.type = OBJ_BUILTIN, .data.builtin_type = BUILTIN_SLEEP};
static const Object fixed_time_diff = {.type = OBJ_BUILTIN, .data.builtin_type = BUILTIN_TIME_DIFF};

// ループ制御関数定数オブジェクト
static const Object fixed_dotimes   = {.type = OBJ_BUILTIN, .data.builtin_type = BUILTIN_DOTIMES};

// 固定オブジェクトへのアクセス関数（const修飾子を適切に処理）
Object* get_nil(void) { return (Object*)&fixed_nil; }
Object* get_true(void) { return (Object*)&fixed_true; }
Object* get_false(void) { return (Object*)&fixed_false; }
Object* get_void(void) { return (Object*)&fixed_void; }

// 演算子定数オブジェクトへのアクセス関数
Object* get_plus(void) { return (Object*)&fixed_plus; }
Object* get_minus(void) { return (Object*)&fixed_minus; }
Object* get_asterisk(void) { return (Object*)&fixed_asterisk; }
Object* get_slash(void) { return (Object*)&fixed_slash; }
Object* get_eq(void) { return (Object*)&fixed_eq; }
Object* get_lt(void) { return (Object*)&fixed_lt; }
Object* get_gt(void) { return (Object*)&fixed_gt; }
Object* get_lte(void) { return (Object*)&fixed_lte; }
Object* get_gte(void) { return (Object*)&fixed_gte; }

// 組み込み関数定数オブジェクトへのアクセス関数
Object* get_print(void) { return (Object*)&fixed_print; }
Object* get_println(void) { return (Object*)&fixed_println; }
Object* get_str(void) { return (Object*)&fixed_str; }
Object* get_length(void) { return (Object*)&fixed_length; }
Object* get_boolp(void) { return (Object*)&fixed_boolp; }

// タイマー関数定数オブジェクトへのアクセス関数
Object* get_now(void) { return (Object*)&fixed_now; }
Object* get_sleep(void) { return (Object*)&fixed_sleep; }
Object* get_time_diff(void) { return (Object*)&fixed_time_diff; }

// ループ制御関数定数オブジェクトへのアクセス関数
Object* get_dotimes(void) { return (Object*)&fixed_dotimes; }


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

Object* make_nil(void) {
    return obj_nil;  // 固定オブジェクトを返す
}

Object* make_true(void) {
    return obj_true;  // 固定オブジェクトを返す
}

Object* make_false(void) {
    return obj_false;  // 固定オブジェクトを返す
}

Object* make_operator(OperatorType op_type) {
    Object* obj = object_pool_alloc();
    if (!obj) return NULL;
    memset(obj, 0, sizeof(Object));  // 全体をゼロクリア
    obj->type = OBJ_OPERATOR;
    obj->data.operator_type = op_type;
    return obj;
}

Object* make_builtin(BuiltinType builtin_type) {
    Object* obj = object_pool_alloc();
    if (!obj) return NULL;
    memset(obj, 0, sizeof(Object));  // 全体をゼロクリア
    obj->type = OBJ_BUILTIN;
    obj->data.builtin_type = builtin_type;
    return obj;
}

//------------------------------------------
// 演算子定数オブジェクト取得関数
//------------------------------------------
Object* get_operator_symbol(const char* op) {
    if (strcmp(op, "+") == 0) return obj_plus;
    if (strcmp(op, "-") == 0) return obj_minus;
    if (strcmp(op, "*") == 0) return obj_asterisk;
    if (strcmp(op, "/") == 0) return obj_slash;
    if (strcmp(op, "=") == 0) return obj_eq;
    if (strcmp(op, "<") == 0) return obj_lt;
    if (strcmp(op, ">") == 0) return obj_gt;
    if (strcmp(op, "<=") == 0) return obj_lte;
    if (strcmp(op, ">=") == 0) return obj_gte;
    return NULL;  // 見つからない場合
}

//------------------------------------------
// 型チェック関数
//------------------------------------------
bool is_nil(Object* obj) { return obj == &fixed_nil; }
bool is_number(Object* obj) { return obj && obj->type == OBJ_NUMBER; }
bool is_string(Object* obj) { return obj && obj->type == OBJ_STRING; }
bool is_symbol(Object* obj) { return obj && obj->type == OBJ_SYMBOL; }
bool is_cons(Object* obj) { return obj && obj->type == OBJ_CONS; }
bool is_function(Object* obj) { return obj && obj->type == OBJ_FUNCTION; }
bool is_lambda(Object* obj) { return obj && obj->type == OBJ_LAMBDA; }
bool is_operator(Object* obj) { return obj && obj->type == OBJ_OPERATOR; }
bool is_builtin(Object* obj) { return obj && obj->type == OBJ_BUILTIN; }

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

OperatorType obj_operator_type(Object* obj) {
    return is_operator(obj) ? obj->data.operator_type : OP_PLUS; // デフォルト値
}

const char* obj_operator_name(Object* obj) {
    if (!is_operator(obj)) return "";
    switch (obj->data.operator_type) {
        case OP_PLUS:     return "+";
        case OP_MINUS:    return "-";
        case OP_ASTERISK: return "*";
        case OP_SLASH:    return "/";
        case OP_EQ:       return "=";
        case OP_LT:       return "<";
        case OP_GT:       return ">";
        case OP_LTE:      return "<=";
        case OP_GTE:      return ">=";
        default:          return "";
    }
}

BuiltinType obj_builtin_type(Object* obj) {
    return is_builtin(obj) ? obj->data.builtin_type : BUILTIN_PRINT; // デフォルト値
}

const char* obj_builtin_name(Object* obj) {
    if (!is_builtin(obj)) return "";
    switch (obj->data.builtin_type) {
        case BUILTIN_PRINT:    return "print";
        case BUILTIN_PRINTLN:  return "println";
        case BUILTIN_STR:      return "str";
        case BUILTIN_LENGTH:   return "length";
        case BUILTIN_BOOLP:    return "bool?";
        case BUILTIN_NOW:      return "now";
        case BUILTIN_SLEEP:    return "sleep";
        case BUILTIN_TIME_DIFF: return "time-diff";
        default:               return "";
    }
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
        case OBJ_OPERATOR:
            printf("%s", obj_operator_name(obj));
            break;
        case OBJ_BUILTIN:
            printf("%s", obj_builtin_name(obj));
            break;
        case OBJ_VOID:
            printf("#<void>");
            break;
    }
}