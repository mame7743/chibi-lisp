
// parser.h
// パーサのインターフェース定義。

#ifndef __PARSER_H__
#define __PARSER_H__
#include <stdint.h>
#include <stddef.h>
#include "tokenizer.h"
#include "object.h"

// パーサー関数
Object* parse(const char* src);
Object* parse_expression(TokenArray* tokens, size_t* index);

#endif // __PARSER_H__