#include <unity.h>
#include "../src/tokenizer.h"

void setUp(void) {}
void tearDown(void) {}

void test_simple_tokenization(void) {
    const char* input = "(+ 1 2)";
    TokenArray *tokens = tokenize(input);
    TEST_ASSERT_NOT_NULL(tokens);
    TEST_ASSERT_EQUAL_INT(5, tokens->size);
    TEST_ASSERT_EQUAL_STRING("(", tokens->tokens[0].value);
    TEST_ASSERT_EQUAL_STRING("+", tokens->tokens[1].value);
    TEST_ASSERT_EQUAL_STRING("1", tokens->tokens[2].value);
    TEST_ASSERT_EQUAL_STRING("2", tokens->tokens[3].value);
    TEST_ASSERT_EQUAL_STRING(")", tokens->tokens[4].value);
    free_token_array(tokens);
}

void test_nested_tokenization(void) {
    const char* input = "(* (+ 1 2) 3)";
    TokenArray *tokens = tokenize(input);
    TEST_ASSERT_NOT_NULL(tokens);
    TEST_ASSERT_EQUAL_INT(9, tokens->size);
    TEST_ASSERT_EQUAL_STRING("(", tokens->tokens[0].value);
    TEST_ASSERT_EQUAL_STRING("*", tokens->tokens[1].value);
    TEST_ASSERT_EQUAL_STRING("(", tokens->tokens[2].value);
    TEST_ASSERT_EQUAL_STRING("+", tokens->tokens[3].value);
    TEST_ASSERT_EQUAL_STRING("1", tokens->tokens[4].value);
    TEST_ASSERT_EQUAL_STRING("2", tokens->tokens[5].value);
    TEST_ASSERT_EQUAL_STRING(")", tokens->tokens[6].value);
    TEST_ASSERT_EQUAL_STRING("3", tokens->tokens[7].value);
    TEST_ASSERT_EQUAL_STRING(")", tokens->tokens[8].value);
    free_token_array(tokens);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_simple_tokenization);
    RUN_TEST(test_nested_tokenization);
    return UNITY_END();
}
