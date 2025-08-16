#include <unity.h>
#include "../src/tokenizer.h"

// �O���[�o���ϐ��Ńg�[�N���z����Ǘ�
static TokenArray* current_tokens = NULL;

void setUp(void) {
    current_tokens = NULL;  // �����I�ɏ�����
}
void tearDown(void) {
    // �e�X�g�I�����ɕK�������������
    if (current_tokens != NULL) {
        free_token_array(current_tokens);
        current_tokens = NULL;
    }
}

//------------------------------------------
// �����́iTokenizer�j�̃e�X�g
//------------------------------------------

void test_tokens(const char* input, int expected_size, TokenKind kinds[], const char* values[]) {
    // �O��̃g�[�N�����c���Ă���ꍇ�͉��
    if (current_tokens != NULL) {
        free_token_array(current_tokens);
        current_tokens = NULL;
    }

    current_tokens = tokenize(input);

    TEST_ASSERT_NOT_NULL(current_tokens);
    TEST_ASSERT_EQUAL_INT(expected_size, current_tokens->size);

    // �e�g�[�N�����`�F�b�N�i�������A�N�Z�X���@�j
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

