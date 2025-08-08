#include <unity.h>
#include "../src/tokenizer.h"

void setUp(void) {}
void tearDown(void) {}

//------------------------------------------
// 字句解析（Tokenizer）のテスト
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

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_simple_tokenization);
    RUN_TEST(test_nested_tokenization);
    return UNITY_END();
}
