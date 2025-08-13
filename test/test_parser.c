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

//------------------------------------------
// パーサーのテスト
//------------------------------------------

void test_parse_number(void) {
    Object* result = parse("42");

    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_TRUE(is_number(result));
    TEST_ASSERT_EQUAL(42, obj_number_value(result));
}

void test_parse_symbol(void) {
    Object* result = parse("hello");

    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_TRUE(is_symbol(result));
    TEST_ASSERT_EQUAL_STRING("hello", obj_symbol_name(result));
}

void test_parse_simple_list(void) {
    Object* result = parse("(+ 1 2)");

    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_TRUE(is_cons(result));

    // 最初の要素は+シンボル
    Object* first = obj_car(result);
    TEST_ASSERT_TRUE(is_symbol(first));
    TEST_ASSERT_EQUAL_STRING("+", obj_symbol_name(first));

    // 2番目の要素は1
    Object* rest = obj_cdr(result);
    TEST_ASSERT_TRUE(is_cons(rest));
    Object* second = obj_car(rest);
    TEST_ASSERT_TRUE(is_number(second));
    TEST_ASSERT_EQUAL(1, obj_number_value(second));
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_parse_number);
    RUN_TEST(test_parse_symbol);
    RUN_TEST(test_parse_simple_list);

    return UNITY_END();
}
