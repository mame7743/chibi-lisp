// test_boolean.c
// �^�U�l�Ɋւ���e�X�g

#include "unity.h"
#include "../src/object.h"
#include "../src/tokenizer.h"
#include "../src/parser.h"
#include "../src/eval.h"

void setUp(void) {
    // �e�X�g�O�̏�����
    object_system_init();
    evaluator_init();
}

void tearDown(void) {
    // �e�X�g��̃N���[���A�b�v
    evaluator_shutdown();
}

// �^�U�l�̊�{�e�X�g
void test_boolean_literals(void) {
    Object* result;

    // t (true) �̃e�X�g
    result = eval_string("t");
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL(OBJ_BOOL, result->type);
    TEST_ASSERT_EQUAL_PTR(obj_true, result);

    // nil (false) �̃e�X�g
    result = eval_string("nil");
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL(OBJ_NIL, result->type);
    TEST_ASSERT_EQUAL_PTR(obj_nil, result);

    // true �̃e�X�g�i�݊����j
    result = eval_string("true");
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL(OBJ_BOOL, result->type);
    TEST_ASSERT_EQUAL_PTR(obj_true, result);

    // false �̃e�X�g�i�݊����j - false��nil�Ɠ���
    result = eval_string("false");
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL(OBJ_NIL, result->type);
    TEST_ASSERT_EQUAL_PTR(obj_nil, result);
}

// bool? �q��̃e�X�g
void test_bool_predicate(void) {
    Object* result;

    // (bool? t) -> t
    result = eval_string("(bool? t)");
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_PTR(obj_true, result);

    // (bool? nil) -> nil (nil�͐^�U�l�^�ł͂Ȃ�)
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

// ��r���Z�̃e�X�g
void test_comparison_operators(void) {
    Object* result;

    // (= 1 1) -> t
    result = eval_string("(= 1 1)");
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_PTR(obj_true, result);

    // (= 1 2) -> nil (false�ł͂Ȃ�nil)
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

// �^�U�l�̕�����ϊ��e�X�g
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

// �g�[�N�i�C�U�e�X�g
void test_boolean_tokenization(void) {
    TokenArray* tokens;

    // "t" �̃g�[�N����
    tokens = tokenize("t");
    TEST_ASSERT_NOT_NULL(tokens);
    TEST_ASSERT_EQUAL(1, tokens->size);
    TEST_ASSERT_EQUAL(TOKEN_TRUE, tokens->tokens[0].kind);
    TEST_ASSERT_EQUAL_STRING("t", tokens->tokens[0].value);
    free_token_array(tokens);

    // "nil" �̃g�[�N����
    tokens = tokenize("nil");
    TEST_ASSERT_NOT_NULL(tokens);
    TEST_ASSERT_EQUAL(1, tokens->size);
    TEST_ASSERT_EQUAL(TOKEN_NIL, tokens->tokens[0].kind);
    TEST_ASSERT_EQUAL_STRING("nil", tokens->tokens[0].value);
    free_token_array(tokens);

    // "true" �̃g�[�N����
    tokens = tokenize("true");
    TEST_ASSERT_NOT_NULL(tokens);
    TEST_ASSERT_EQUAL(1, tokens->size);
    TEST_ASSERT_EQUAL(TOKEN_TRUE, tokens->tokens[0].kind);
    TEST_ASSERT_EQUAL_STRING("true", tokens->tokens[0].value);
    free_token_array(tokens);

    // "false" �̃g�[�N����
    tokens = tokenize("false");
    TEST_ASSERT_NOT_NULL(tokens);
    TEST_ASSERT_EQUAL(1, tokens->size);
    TEST_ASSERT_EQUAL(TOKEN_FALSE, tokens->tokens[0].kind);
    TEST_ASSERT_EQUAL_STRING("false", tokens->tokens[0].value);
    free_token_array(tokens);
}

// �p�[�T�e�X�g
void test_boolean_parsing(void) {
    Object* result;

    // "t" �̃p�[�X
    result = parse("t");
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL(OBJ_BOOL, result->type);
    TEST_ASSERT_EQUAL_PTR(obj_true, result);

    // "nil" �̃p�[�X
    result = parse("nil");
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL(OBJ_NIL, result->type);
    TEST_ASSERT_EQUAL_PTR(obj_nil, result);

    // "true" �̃p�[�X
    result = parse("true");
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL(OBJ_BOOL, result->type);
    TEST_ASSERT_EQUAL_PTR(obj_true, result);

    // "false" �̃p�[�X
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
