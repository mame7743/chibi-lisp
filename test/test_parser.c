#include <unity.h>
#include "../src/parser.h"
#include "../src/tokenizer.h"
#include <string.h>

static ASTNode* test_ast = NULL;

void setUp(void) {
    heap_init();
    test_ast = NULL;
}

void tearDown(void) {
    if (test_ast != NULL) {
        free_ast(test_ast);
        test_ast = NULL;
    }
}

//------------------------------------------
// パーサーのテスト
//------------------------------------------

void test_simple_ast_construction(void) {
    // "(+ 1 2)" をパース
    TokenArray* tokens = tokenize("(+ 1 2)");
    TEST_ASSERT_NOT_NULL(tokens);

    test_ast = parse(tokens);
    TEST_ASSERT_NOT_NULL(test_ast);

    // 結果は (+ 1 2) の構造
    TEST_ASSERT_EQUAL_INT(AST_CONS, test_ast->type);

    // 先頭要素は "+" シンボル
    ASTNode* op = test_ast->data.cons.car;
    TEST_ASSERT_NOT_NULL(op);
    TEST_ASSERT_EQUAL_INT(AST_SYMBOL, op->type);
    TEST_ASSERT_EQUAL_STRING("+", op->data.symbol);

    // 引数リスト
    ASTNode* args = test_ast->data.cons.cdr;
    TEST_ASSERT_NOT_NULL(args);
    TEST_ASSERT_EQUAL_INT(AST_CONS, args->type);

    // 最初の引数: 1
    ASTNode* arg1 = args->data.cons.car;
    TEST_ASSERT_NOT_NULL(arg1);
    TEST_ASSERT_EQUAL_INT(AST_NUMBER, arg1->type);
    TEST_ASSERT_EQUAL_INT(1, arg1->data.number);

    free_token_array(tokens);
}

void test_nested_ast_construction(void) {
    // "(* (+ 1 2) 3)" をパース
    TokenArray* tokens = tokenize("(* (+ 1 2) 3)");
    TEST_ASSERT_NOT_NULL(tokens);

    test_ast = parse(tokens);
    TEST_ASSERT_NOT_NULL(test_ast);

    // 結果は (* (+ 1 2) 3) の構造
    TEST_ASSERT_EQUAL_INT(AST_CONS, test_ast->type);

    // 先頭要素は "*" シンボル
    ASTNode* op = test_ast->data.cons.car;
    TEST_ASSERT_NOT_NULL(op);
    TEST_ASSERT_EQUAL_INT(AST_SYMBOL, op->type);
    TEST_ASSERT_EQUAL_STRING("*", op->data.symbol);

    // 引数リスト
    ASTNode* args = test_ast->data.cons.cdr;
    TEST_ASSERT_NOT_NULL(args);

    // 最初の引数: (+ 1 2)
    ASTNode* arg1 = args->data.cons.car;
    TEST_ASSERT_NOT_NULL(arg1);
    TEST_ASSERT_EQUAL_INT(AST_CONS, arg1->type);

    // 内側の演算子 "+"
    ASTNode* inner_op = arg1->data.cons.car;
    TEST_ASSERT_EQUAL_STRING("+", inner_op->data.symbol);

    free_token_array(tokens);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_simple_ast_construction);
    RUN_TEST(test_nested_ast_construction);
    return UNITY_END();
}
    return UNITY_END();
}
