#include <unity.h>
#include "../src/tokenizer.h"
#include "../src/parser.h"
#include "../src/value.h"

void setUp(void) {}
void tearDown(void) {}

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

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_simple_ast_construction);
    RUN_TEST(test_nested_ast_construction);
    return UNITY_END();
}
