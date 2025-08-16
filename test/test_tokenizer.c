#include <unity.h>
#include "../src/tokenizer.h"

// グローバル変数でトークン配列を管理
static TokenArray* current_tokens = NULL;

void setUp(void) {
    current_tokens = NULL;  // 明示的に初期化
}
void tearDown(void) {
    // テスト終了時に必ずメモリを解放
    if (current_tokens != NULL) {
        free_token_array(current_tokens);
        current_tokens = NULL;
    }
}

//------------------------------------------
// 字句解析（Tokenizer）のテスト
//------------------------------------------

void test_tokens(const char* input, int expected_size, TokenKind kinds[], const char* values[]) {
    // 前回のトークンが残っている場合は解放
    if (current_tokens != NULL) {
        free_token_array(current_tokens);
        current_tokens = NULL;
    }

    current_tokens = tokenize(input);

    TEST_ASSERT_NOT_NULL(current_tokens);
    TEST_ASSERT_EQUAL_INT(expected_size, current_tokens->size);

    // 各トークンをチェック（正しいアクセス方法）
    for (int i = 0; i < expected_size; i++) {
        TEST_ASSERT_EQUAL_STRING(values[i], current_tokens->tokens[i].value);
        TEST_ASSERT_EQUAL_INT(kinds[i], current_tokens->tokens[i].kind);
    }
}

void test_simple_tokenization(void) {
    TokenKind kinds[]    = {TOKEN_LPAREN, TOKEN_PLUS, TOKEN_NUMBER, TOKEN_NUMBER, TOKEN_RPAREN};
    const char* values[] = {"(", "+", "1", "2", ")"};
    test_tokens("(+ 1 2)", 5, kinds, values);
}

void test_nested_tokenization(void) {
    TokenKind kinds[]    = {TOKEN_LPAREN, TOKEN_ASTERISK, TOKEN_LPAREN, TOKEN_PLUS, TOKEN_NUMBER, TOKEN_NUMBER, TOKEN_RPAREN, TOKEN_NUMBER, TOKEN_RPAREN};
    const char* values[] = {"(", "*", "(", "+", "1", "2", ")", "3", ")"};
    test_tokens("(* (+ 1 2) 3)", 9, kinds, values);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_simple_tokenization);
    RUN_TEST(test_nested_tokenization);
    return UNITY_END();
}

