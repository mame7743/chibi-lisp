#ifndef OBJECT_H
#define OBJECT_H

#include "chibi_lisp.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// LISPオブジェクトの型
typedef enum {
    OBJ_NIL,        // nil
    OBJ_BOOL,       // t / nil (boolean)
    OBJ_NUMBER,     // 数値 (整数)
    OBJ_STRING,     // 文字列
    OBJ_SYMBOL,     // シンボル
    OBJ_CONS,       // コンスセル (car . cdr)
    OBJ_FUNCTION,   // 関数オブジェクト
    OBJ_LAMBDA,     // ラムダ式
    OBJ_OPERATOR,   // 演算子 (+, -, *, /, =, <, >, <=, >=)
    OBJ_BUILTIN,    // 組み込み関数 (print, println, str, length, bool?)
    OBJ_VOID,       // void (REPLで表示しない戻り値)
} ObjectType;

// 演算子の種類
typedef enum {
    OP_PLUS,        // +
    OP_MINUS,       // -
    OP_ASTERISK,    // *
    OP_SLASH,       // /
    OP_EQ,          // =
    OP_LT,          // <
    OP_GT,          // >
    OP_LTE,         // <=
    OP_GTE,         // >=
} OperatorType;

// 組み込み関数の種類
typedef enum {
    BUILTIN_PRINT,    // print
    BUILTIN_PRINTLN,  // println
    BUILTIN_STR,      // str
    BUILTIN_LENGTH,   // length
    BUILTIN_BOOLP,    // bool?
    // タイマー関数
    BUILTIN_NOW,      // now (現在時刻のミリ秒取得)
    BUILTIN_SLEEP,    // sleep (指定秒数待機)
    BUILTIN_TIME_DIFF, // time-diff (時間差計算)
    // ループ制御関数
    BUILTIN_DOTIMES,   // dotime (指定回数繰り返し)
} BuiltinType;

// 前方宣言
typedef struct Object Object;

// LISPオブジェクト構造体
typedef struct Object {
    ObjectType type;
    union {
        // 数値
        int number;

        // 文字列
        struct {
            char* text;
            size_t length;
        } string;

        // シンボル
        struct {
            char* name;
            size_t length;
        } symbol;

        // コンスセル
        struct {
            Object* car;
            Object* cdr;
        } cons;

        // 関数
        struct {
            Object* (*native_func)(Object* args);  // ネイティブ関数
            Object* params;                        // パラメータリスト
            Object* body;                          // 関数本体
        } function;

        // 演算子
        OperatorType operator_type;

        // 組み込み関数
        BuiltinType builtin_type;
    } data;
} Object;

// 固定オブジェクトへのアクセス関数（const修飾子を適切に処理）
Object* get_nil(void);
Object* get_true(void);
Object* get_false(void);
Object* get_void(void);

// 演算子定数オブジェクトへのアクセス関数
Object* get_plus(void);     // +
Object* get_minus(void);    // -
Object* get_asterisk(void); // *
Object* get_slash(void);    // /
Object* get_eq(void);       // =
Object* get_lt(void);       // <
Object* get_gt(void);       // >
Object* get_lte(void);      // <=
Object* get_gte(void);      // >=

// 組み込み関数定数オブジェクトへのアクセス関数
Object* get_print(void);    // print
Object* get_println(void);  // println
Object* get_str(void);      // str
Object* get_length(void);   // length
Object* get_boolp(void);    // bool?
// タイマー関数定数オブジェクトへのアクセス関数
Object* get_now(void);      // now
Object* get_sleep(void);    // sleep
Object* get_time_diff(void); // time-diff
// ループ制御関数定数オブジェクトへのアクセス関数
Object* get_dotimes(void);   // dotime

// 互換性のためのマクロ定義（徐々に関数に移行）
#define obj_nil get_nil()
#define obj_true get_true()
#define obj_false get_false()
#define obj_void get_void()
#define obj_plus get_plus()
#define obj_minus get_minus()
#define obj_asterisk get_asterisk()
#define obj_slash get_slash()
#define obj_eq get_eq()
#define obj_lt get_lt()
#define obj_gt get_gt()
#define obj_lte get_lte()
#define obj_gte get_gte()
#define obj_print get_print()
#define obj_println get_println()
#define obj_str get_str()
#define obj_length get_length()
#define obj_boolp get_boolp()
#define obj_now get_now()
#define obj_sleep get_sleep()
#define obj_time_diff get_time_diff()
#define obj_dotimes get_dotimes()

// オブジェクトシステム初期化
void object_system_init(void);
void object_system_cleanup(void);

// オブジェクト作成関数
Object* make_number(int value);
Object* make_string(const char* text);
Object* make_symbol(const char* name);
Object* make_cons(Object* car, Object* cdr);
Object* make_function(Object* (*func)(Object*));
Object* make_lambda(Object* params, Object* body);
Object* make_operator(OperatorType op_type);
Object* make_builtin(BuiltinType builtin_type);

// 型チェック関数
bool is_nil(Object* obj);
bool is_number(Object* obj);
bool is_string(Object* obj);
bool is_symbol(Object* obj);
bool is_cons(Object* obj);
bool is_function(Object* obj);
bool is_lambda(Object* obj);
bool is_operator(Object* obj);
bool is_builtin(Object* obj);

// アクセサ関数
int obj_number_value(Object* obj);
const char* obj_string_text(Object* obj);
const char* obj_symbol_name(Object* obj);
Object* obj_car(Object* obj);
Object* obj_cdr(Object* obj);
OperatorType obj_operator_type(Object* obj);
const char* obj_operator_name(Object* obj);
BuiltinType obj_builtin_type(Object* obj);
const char* obj_builtin_name(Object* obj);

// リスト操作
Object* obj_list_length(Object* list);
Object* obj_list_nth(Object* list, int n);
Object* obj_list_append(Object* list1, Object* list2);

// デバッグ用
void object_dump(Object* obj);

#endif // OBJECT_H