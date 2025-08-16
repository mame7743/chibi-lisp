// test_boolean.c
// 真偽値に関するテスト

#include "unity.h"
#include "../src/object.h"
#include "../src/tokenizer.h"
#include "../src/parser.h"
#include "../src/eval.h"

void setUp(void) {
    // テスト前の初期化
    object_system_init();
    evaluator_init();
}

void tearDown(void) {
    // テスト後のクリーンアップ
    evaluator_shutdown();
}

// 真偽値の基本テスト
void test_boolean_literals(void) {
    Object* result;

    // t (true) のテスト
    result = eval_string("t");
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL(OBJ_BOOL, result->type);
    TEST_ASSERT_EQUAL_PTR(obj_true, result);

    // nil (false) のテスト
    result = eval_string("nil");
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL(OBJ_NIL, result->type);
    TEST_ASSERT_EQUAL_PTR(obj_nil, result);

    // true のテスト（互換性）
    result = eval_string("true");
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL(OBJ_BOOL, result->type);
    TEST_ASSERT_EQUAL_PTR(obj_true, result);

    // false のテスト（互換性） - falseはnilと同じ
    result = eval_string("false");
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL(OBJ_NIL, result->type);
    TEST_ASSERT_EQUAL_PTR(obj_nil, result);
}

// bool? 述語のテスト
void test_bool_predicate(void) {
    Object* result;

    // (bool? t) -> t
    result = eval_string("(bool? t)");
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_PTR(obj_true, result);

    // (bool? nil) -> nil (nilは真偽値型ではない)
    result = eval_string("(bool? nil)");
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_PTR(obj_nil, result);

    // (bool? 42) -> nil
    result = eval_string("(bool? 42)");
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_PTR(obj_nil, result);

    // (bool? "string") -> nil
    result = eval_string("(bool? \"string\")");
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_PTR(obj_nil, result);
}

// 比較演算のテスト
void test_comparison_operators(void) {
    Object* result;

    // (= 1 1) -> t
    result = eval_string("(= 1 1)");
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_PTR(obj_true, result);

    // (= 1 2) -> nil (falseではなくnil)
    result = eval_string("(= 1 2)");
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_PTR(obj_nil, result);

    // (< 1 2) -> t
    result = eval_string("(< 1 2)");
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_PTR(obj_true, result);

    // (> 2 1) -> t
    result = eval_string("(> 2 1)");
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_PTR(obj_true, result);
}

// 真偽値の文字列変換テスト
void test_boolean_string_conversion(void) {
    Object* result;

    // (str t) -> "t"
    result = eval_string("(str t)");
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL(OBJ_STRING, result->type);
    TEST_ASSERT_EQUAL_STRING("t", result->data.string.text);

    // (str nil) -> "nil"
    result = eval_string("(str nil)");
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL(OBJ_STRING, result->type);
    TEST_ASSERT_EQUAL_STRING("nil", result->data.string.text);

    // (str "result: " t " or " nil) -> "result: t or nil"
    result = eval_string("(str \"result: \" t \" or \" nil)");
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL(OBJ_STRING, result->type);
    TEST_ASSERT_EQUAL_STRING("result: t or nil", result->data.string.text);
}

// トークナイザテスト
void test_boolean_tokenization(void) {
    TokenArray* tokens;

    // "t" のトークン化
    tokens = tokenize("t");
    TEST_ASSERT_NOT_NULL(tokens);
    TEST_ASSERT_EQUAL(1, tokens->size);
    TEST_ASSERT_EQUAL(TOKEN_TRUE, tokens->tokens[0].kind);
    TEST_ASSERT_EQUAL_STRING("t", tokens->tokens[0].value);
    free_token_array(tokens);

    // "nil" のトークン化
    tokens = tokenize("nil");
    TEST_ASSERT_NOT_NULL(tokens);
    TEST_ASSERT_EQUAL(1, tokens->size);
    TEST_ASSERT_EQUAL(TOKEN_NIL, tokens->tokens[0].kind);
    TEST_ASSERT_EQUAL_STRING("nil", tokens->tokens[0].value);
    free_token_array(tokens);

    // "true" のトークン化
    tokens = tokenize("true");
    TEST_ASSERT_NOT_NULL(tokens);
    TEST_ASSERT_EQUAL(1, tokens->size);
    TEST_ASSERT_EQUAL(TOKEN_TRUE, tokens->tokens[0].kind);
    TEST_ASSERT_EQUAL_STRING("true", tokens->tokens[0].value);
    free_token_array(tokens);

    // "false" のトークン化
    tokens = tokenize("false");
    TEST_ASSERT_NOT_NULL(tokens);
    TEST_ASSERT_EQUAL(1, tokens->size);
    TEST_ASSERT_EQUAL(TOKEN_FALSE, tokens->tokens[0].kind);
    TEST_ASSERT_EQUAL_STRING("false", tokens->tokens[0].value);
    free_token_array(tokens);
}

// パーサテスト
void test_boolean_parsing(void) {
    Object* result;

    // "t" のパース
    result = parse("t");
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL(OBJ_BOOL, result->type);
    TEST_ASSERT_EQUAL_PTR(obj_true, result);

    // "nil" のパース
    result = parse("nil");
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL(OBJ_NIL, result->type);
    TEST_ASSERT_EQUAL_PTR(obj_nil, result);

    // "true" のパース
    result = parse("true");
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL(OBJ_BOOL, result->type);
    TEST_ASSERT_EQUAL_PTR(obj_true, result);

    // "false" のパース
    result = parse("false");
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL(OBJ_NIL, result->type);
    TEST_ASSERT_EQUAL_PTR(obj_nil, result);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_boolean_literals);
    RUN_TEST(test_bool_predicate);
    RUN_TEST(test_comparison_operators);
    RUN_TEST(test_boolean_string_conversion);
    RUN_TEST(test_boolean_tokenization);
    RUN_TEST(test_boolean_parsing);

    return UNITY_END();
}
