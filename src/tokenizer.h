#ifndef __TOKENIZER_H__
#define __TOKENIZER_H__

#include <stdint.h>
#include <stddef.h>

typedef enum {
    TOKEN_LPAREN = 0,  // (
    TOKEN_RPAREN,      // )
    TOKEN_SYMBOL,      // シンボル
    TOKEN_LAMBDA,      // ラムダ式
    TOKEN_IF,          // if式
    TOKEN_QUOTE,       // quote式
    TOKEN_LOOP,        // loop式
    TOKEN_DEFINE,      // define
    TOKEN_SET,         // set!
    TOKEN_NIL,         // nil
    TOKEN_TRUE,        // true
    TOKEN_FALSE,       // false
    TOKEN_NUMBER,      // 数値
    TOKEN_STRING,      // 文字列
    TOKEN_PLUS,        // +
    TOKEN_MINUS,       // -
    TOKEN_ASTERISK,    // *
    TOKEN_SLASH,       // /
    TOKEN_EQ,          // =
    TOKEN_LT,          // <
    TOKEN_GT,          // >
    TOKEN_LTE,         // <=
    TOKEN_GTE,         // >=
    // 組み込み関数
    TOKEN_PRINT,       // print
    TOKEN_PRINTLN,     // println
    TOKEN_STR,         // str
    TOKEN_LENGTH,      // length
    TOKEN_BOOLP,       // bool?
    // タイマー関数
    TOKEN_NOW,         // now (現在時刻のミリ秒取得)
    TOKEN_SLEEP,       // sleep (指定秒数待機)
    TOKEN_TIME_DIFF,   // time-diff (時間差計算)
    // ループ制御関数
    TOKEN_DOTIMES,      // dotimes (指定回数繰り返し)
} TokenKind;

typedef struct {
    TokenKind kind;  // Type of the token
    char* value;     // セミコロンを追加
} Token;

typedef struct {
    size_t size;
    Token* tokens;
} TokenArray;

// 関数宣言
TokenArray* tokenize(const char* input);
void free_token_array(TokenArray* tokens);

#endif  // __TOKENIZER_H__