
// value.h
// Lispデータ型の定義。

#ifndef VALUE_H
#define VALUE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// 値の型を定義
typedef enum {
    VALUE_NIL,     // 空値
    VALUE_BOOL,    // 真偽値
    VALUE_INT,     // 整数
    VALUE_STRING,  // 文字列
    VALUE_CONS,    // リストの要素
    VALUE_SYMBOL,  // シンボル（変数名、関数名など）
} ValueKind;

// 前方宣言
typedef struct Value Value;

typedef struct Value {
    ValueKind kind;
    union {
        int number;
        const char *symbol;
        struct {
            Value *car;
            Value *cdr;
        } cons;
        struct {
            char *text;
            size_t len;
        } string;
    } data;
} Value;

// 固定値の宣言（定義はvalue.cで行う）
extern const Value* nil;
extern const Value* true_value;
extern const Value* false_value;

// 関数宣言
Value* make_int_value(int value);
Value* make_string_value(const char* str);
Value* make_cons_value(Value* car, Value* cdr);
Value* make_symbol_value(const char* symbol);

#endif // VALUE_H