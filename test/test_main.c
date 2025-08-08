// test_main.c
// ユニットテストやREPLの自動テスト用。

#include <unity.h>
#include "../src/tokenizer.h"
#include "../src/parser.h"
#include "../src/eval.h"
#include "../src/env.h"
#include "../src/value.h"
#include "../src/primitive.h"

void setUp(void) {
    // 各テスト前の初期化処理
}

void tearDown(void) {
    // 各テスト後のクリーンアップ処理
}

//------------------------------------------
// 字句解析（Lexer）のテスト
//------------------------------------------
void test_simple_tokenization(void) {
    const char* input = "(+ 1 2)";
    TokenArray tokens = tokenize(input);
    
    TEST_ASSERT_EQUAL_INT(5, tokens.size);
    TEST_ASSERT_EQUAL_STRING("(", tokens.tokens[0].text);
    TEST_ASSERT_EQUAL_STRING("+", tokens.tokens[1].text);
    TEST_ASSERT_EQUAL_STRING("1", tokens.tokens[2].text);
    TEST_ASSERT_EQUAL_STRING("2", tokens.tokens[3].text);
    TEST_ASSERT_EQUAL_STRING(")", tokens.tokens[4].text);
}

void test_nested_tokenization(void) {
    const char* input = "(* (+ 1 2) 3)";
    TokenArray tokens = tokenize(input);
    
    TEST_ASSERT_EQUAL_INT(9, tokens.size);
    TEST_ASSERT_EQUAL_STRING("(", tokens.tokens[0].text);
    TEST_ASSERT_EQUAL_STRING("*", tokens.tokens[1].text);
    TEST_ASSERT_EQUAL_STRING("(", tokens.tokens[2].text);
    TEST_ASSERT_EQUAL_STRING("+", tokens.tokens[3].text);
    TEST_ASSERT_EQUAL_STRING("1", tokens.tokens[4].text);
    TEST_ASSERT_EQUAL_STRING("2", tokens.tokens[5].text);
    TEST_ASSERT_EQUAL_STRING(")", tokens.tokens[6].text);
    TEST_ASSERT_EQUAL_STRING("3", tokens.tokens[7].text);
    TEST_ASSERT_EQUAL_STRING(")", tokens.tokens[8].text);
}

//------------------------------------------
// パーサー（Parser）のテスト
//------------------------------------------
void test_simple_ast_construction(void) {
    const char* input = "(+ 1 2)";
    TokenArray tokens = tokenize(input);
    Value* ast = parse(&tokens);
    
    TEST_ASSERT_NOT_NULL(ast);
    TEST_ASSERT_EQUAL_STRING("+", ast->list->items[0]->symbol);
    TEST_ASSERT_EQUAL_INT(1, ast->list->items[1]->number);
    TEST_ASSERT_EQUAL_INT(2, ast->list->items[2]->number);
}

void test_nested_ast_construction(void) {
    const char* input = "(* (+ 1 2) 3)";
    TokenArray tokens = tokenize(input);
    Value* ast = parse(&tokens);
    
    TEST_ASSERT_NOT_NULL(ast);
    TEST_ASSERT_EQUAL_STRING("*", ast->list->items[0]->symbol);
    
    Value* nested = ast->list->items[1];
    TEST_ASSERT_EQUAL_STRING("+", nested->list->items[0]->symbol);
    TEST_ASSERT_EQUAL_INT(1, nested->list->items[1]->number);
    TEST_ASSERT_EQUAL_INT(2, nested->list->items[2]->number);
    
    TEST_ASSERT_EQUAL_INT(3, ast->list->items[2]->number);
}

//------------------------------------------
// 評価器（Eval）のテスト
//------------------------------------------
void test_addition_evaluation(void) {
    const char* input = "(+ 1 2)";
    TokenArray tokens = tokenize(input);
    Value* ast = parse(&tokens);
    Env* env = env_new();
    env_add_primitives(env);
    
    Value* result = eval(ast, env);
    TEST_ASSERT_EQUAL_INT(3, result->number);
}

void test_complex_arithmetic(void) {
    const char* input = "(* (+ 1 2) (- 5 3))";
    TokenArray tokens = tokenize(input);
    Value* ast = parse(&tokens);
    Env* env = env_new();
    env_add_primitives(env);
    
    Value* result = eval(ast, env);
    TEST_ASSERT_EQUAL_INT(6, result->number);
}

//------------------------------------------
// 組み込み関数のテスト
//------------------------------------------
void test_list_creation(void) {
    const char* input = "(list 1 2 3)";
    TokenArray tokens = tokenize(input);
    Value* ast = parse(&tokens);
    Env* env = env_new();
    env_add_primitives(env);
    
    Value* result = eval(ast, env);
    TEST_ASSERT_NOT_NULL(result->list);
    TEST_ASSERT_EQUAL_INT(3, result->list->size);
    TEST_ASSERT_EQUAL_INT(1, result->list->items[0]->number);
    TEST_ASSERT_EQUAL_INT(2, result->list->items[1]->number);
    TEST_ASSERT_EQUAL_INT(3, result->list->items[2]->number);
}

void test_car_function(void) {
    const char* input = "(car (list 1 2 3))";
    TokenArray tokens = tokenize(input);
    Value* ast = parse(&tokens);
    Env* env = env_new();
    env_add_primitives(env);
    
    Value* result = eval(ast, env);
    TEST_ASSERT_EQUAL_INT(1, result->number);
}

void test_cdr_function(void) {
    const char* input = "(cdr (list 1 2 3))";
    TokenArray tokens = tokenize(input);
    Value* ast = parse(&tokens);
    Env* env = env_new();
    env_add_primitives(env);
    
    Value* result = eval(ast, env);
    TEST_ASSERT_NOT_NULL(result->list);
    TEST_ASSERT_EQUAL_INT(2, result->list->size);
    TEST_ASSERT_EQUAL_INT(2, result->list->items[0]->number);
    TEST_ASSERT_EQUAL_INT(3, result->list->items[1]->number);
}

//------------------------------------------
// 変数定義・ラムダのテスト
//------------------------------------------
void test_variable_definition_and_reference(void) {
    Env* env = env_new();
    env_add_primitives(env);
    
    // Define x
    const char* def_input = "(define x 42)";
    TokenArray def_tokens = tokenize(def_input);
    Value* def_ast = parse(&def_tokens);
    eval(def_ast, env);
    
    // Reference x
    const char* ref_input = "x";
    TokenArray ref_tokens = tokenize(ref_input);
    Value* ref_ast = parse(&ref_tokens);
    Value* result = eval(ref_ast, env);
    
    TEST_ASSERT_EQUAL_INT(42, result->number);
}

void test_lambda_definition_and_call(void) {
    const char* input = "((lambda (a b) (+ a b)) 3 4)";
    TokenArray tokens = tokenize(input);
    Value* ast = parse(&tokens);
    Env* env = env_new();
    env_add_primitives(env);
    
    Value* result = eval(ast, env);
    TEST_ASSERT_EQUAL_INT(7, result->number);
}

int main(void) {
    UNITY_BEGIN();
    
    // Lexer Tests
    RUN_TEST(test_simple_tokenization);
    RUN_TEST(test_nested_tokenization);
    
    // Parser Tests
    RUN_TEST(test_simple_ast_construction);
    RUN_TEST(test_nested_ast_construction);
    
    // Evaluator Tests
    RUN_TEST(test_addition_evaluation);
    RUN_TEST(test_complex_arithmetic);
    
    // Built-in Function Tests
    RUN_TEST(test_list_creation);
    RUN_TEST(test_car_function);
    RUN_TEST(test_cdr_function);
    
    // Variable and Lambda Tests
    RUN_TEST(test_variable_definition_and_reference);
    RUN_TEST(test_lambda_definition_and_call);
    
    return UNITY_END();
}
