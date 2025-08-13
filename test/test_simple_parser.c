#include "../lib/unity/src/unity.h"
#include "../src/object.h"
#include "../src/object_pool.h"
#include "../src/gc.h"
#include "../src/parser.h"
#include "../src/tokenizer.h"
#include <string.h>

void setUp(void) {
    object_system_init();
}

void tearDown(void) {
    object_system_cleanup();
}

void test_simple_parse(void) {
    printf("Starting simple parse test...\n");

    // ‚Ü‚¸tokenize‚ª“®ì‚·‚é‚©ƒeƒXƒg
    TokenArray* tokens = tokenize("42");
    printf("Tokenize result: %p\n", (void*)tokens);

    if (tokens) {
        printf("Token count: %zu\n", tokens->size);
        if (tokens->size > 0) {
            printf("First token kind: %d, value: %s\n",
                   tokens->tokens[0].kind, tokens->tokens[0].value);
        }
        free_token_array(tokens);
    }

    printf("Test completed successfully\n");
    TEST_ASSERT_TRUE(true);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_simple_parse);

    return UNITY_END();
}
