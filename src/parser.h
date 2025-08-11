
// parser.h
// パーサのインターフェース定義。

#ifndef __PARSER_H__
#define __PARSER_H__
#include <stdint.h>
#include <stddef.h>
#include "tokenizer.h"

typedef enum {
    OBJECT_NIL = 0,
    OBJECT_INT,
    OBJECT_CONS,
    OBJECT_STRING,
    OBJECT_HASHENTRY
} ObjKind;

typedef struct Obj {
    ObjKind type;
    uint8_t marked;
    struct Obj* next_free;  // free-list
    union {
        int32_t iv;  // integer
        struct {
            struct Obj *car, *cdr;
        } cons;
        struct {
            void* strptr;
            uint16_t len;
        } s;
        struct {
            struct Obj *key, *val, *next;
        } he;
    } v;
} Obj;

typedef enum {
    AST_CONS,    // リストの構成要素
    AST_NUMBER,  // 数値
    AST_SYMBOL   // シンボル（演算子、変数名など）
} ASTNodeType;

typedef struct ASTNode {
    ASTNodeType type;
    union {
        struct {
            struct ASTNode* car;  // リストの先頭要素
            struct ASTNode* cdr;  // リストの残りの部分
        } cons;
        int number;               // 数値
        char* symbol;             // シンボル（文字列）
    } data;
} ASTNode;

// 関数宣言
ASTNode* make_cons(ASTNode* car, ASTNode* cdr);
ASTNode* make_number(int value);
ASTNode* make_symbol(const char* symbol);
ASTNode* parse(TokenArray* tokens);
void free_ast(ASTNode* node);

#endif // __PARSER_H__