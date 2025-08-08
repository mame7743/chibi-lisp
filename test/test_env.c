#include <unity.h>
#include "../src/tokenizer.h"
#include "../src/parser.h"
#include "../src/eval.h"
#include "../src/env.h"
#include "../src/value.h"
#include "../src/primitive.h"

void setUp(void) {}
void tearDown(void) {}

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

    // クリーンアップ
    env_free(env);
}

void test_lambda_definition_and_call(void) {
    Env* env = env_new();
    env_add_primitives(env);
    
    const char* input = "((lambda (a b) (+ a b)) 3 4)";
    TokenArray tokens = tokenize(input);
    Value* ast = parse(&tokens);
    Value* result = eval(ast, env);
    
    TEST_ASSERT_EQUAL_INT(7, result->number);

    // クリーンアップ
    env_free(env);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_variable_definition_and_reference);
    RUN_TEST(test_lambda_definition_and_call);
    return UNITY_END();
}
