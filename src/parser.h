// parser.h
// パーサのインターフェース定義。

#ifndef __PARSER_H__
#define __PARSER_H__
#include <stdint.h>
#include <stddef.h>
#include "tokenizer.h"
#include "object.h"

// 最初の1式のみ
Object* parse(const char* src);
// トークン列から現在位置 index を更新しつつ次の式を1つパース
Object* parse_expression(TokenArray* tokens, size_t* index);
// 入力中の全トップレベル式をリスト (expr1 expr2 ...) として返す補助
Object* parse_all(const char* src);

#endif // __PARSER_H__