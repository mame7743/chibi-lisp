#ifndef OBJECT_H
#define OBJECT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// LISPオブジェクトの型
typedef enum {
    OBJ_NIL,       // nil
    OBJ_BOOL,      // t / nil (boolean)
    OBJ_NUMBER,    // 数値 (整数)
    OBJ_STRING,    // 文字列
    OBJ_SYMBOL,    // シンボル
    OBJ_CONS,      // コンスセル (car . cdr)
    OBJ_FUNCTION,  // 関数オブジェクト
    OBJ_LAMBDA,    // ラムダ式
} ObjectType;

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
    } data;
} Object;

// 固定オブジェクトへのポインタ（外部からアクセス可能）
extern Object* obj_nil;
extern Object* obj_true;
extern Object* obj_false;

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

// 型チェック関数
bool is_nil(Object* obj);
bool is_number(Object* obj);
bool is_string(Object* obj);
bool is_symbol(Object* obj);
bool is_cons(Object* obj);
bool is_function(Object* obj);
bool is_lambda(Object* obj);

// アクセサ関数
int obj_number_value(Object* obj);
const char* obj_string_text(Object* obj);
const char* obj_symbol_name(Object* obj);
Object* obj_car(Object* obj);
Object* obj_cdr(Object* obj);

// リスト操作
Object* obj_list_length(Object* list);
Object* obj_list_nth(Object* list, int n);
Object* obj_list_append(Object* list1, Object* list2);

// デバッグ用
void object_dump(Object* obj);

#endif // OBJECT_H