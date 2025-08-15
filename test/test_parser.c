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

    // 最初の要素は+演算子オブジェクト
    Object* first = obj_car(result);
    TEST_ASSERT_TRUE(is_operator(first));
    TEST_ASSERT_EQUAL(OP_PLUS, obj_operator_type(first));

    // 2番目の要素は1
    Object* rest = obj_cdr(result);
    TEST_ASSERT_TRUE(is_cons(rest));
    Object* second = obj_car(rest);
    TEST_ASSERT_TRUE(is_number(second));
    TEST_ASSERT_EQUAL(1, obj_number_value(second));
}

// 深いネスト (DEPTH_MAX 付近) の生成ヘルパ
static void build_nested(char *buf, size_t depth, size_t bufsize) {
    size_t pos = 0;
    for (size_t i = 0; i < depth && pos + 1 < bufsize; i++) {
        buf[pos++] = '(';
    }
    if (pos + 2 < bufsize) {
        buf[pos++] = '1';
    }
    for (size_t i = 0; i < depth && pos + 1 < bufsize; i++) {
        buf[pos++] = ')';
    }
    if (pos < bufsize) buf[pos] = '\0';
}

// DEPTH_MAX-1 までのネストはパース成功 (parser.c の DEPTH_MAX と同期必要)
void test_parse_deep_nest_ok(void) {
    const size_t DEPTH = 255; // DEPTH_MAX=256 の前提
    char src[1024];
    build_nested(src, DEPTH, sizeof(src));
    Object *result = parse(src);
    TEST_ASSERT_NOT_NULL(result);
    // 生成形: 最外リストの car が次リスト... という形ではなく、( ( ( 1 ) ) ) -> 最外: ( <inner> ) の一段構造連鎖
    // 各階層: cons(cell(car=次層 or 1, cdr=nil))
    Object *cur = result;
    size_t depth_count = 0;
    while (cur && is_cons(cur)) {
        Object *car = obj_car(cur);
        depth_count++;
        if (is_number(car)) {
            TEST_ASSERT_EQUAL(1, obj_number_value(car));
            break;
        } else {
            TEST_ASSERT_TRUE(is_cons(car));
            cur = car; // 次層へ
        }
    }
    TEST_ASSERT_TRUE(depth_count <= DEPTH);
}

// DEPTH_MAX 超過で安全に失敗 (obj_nil 戻り値を想定)
void test_parse_deep_nest_overflow(void) {
    const size_t DEPTH = 300; // 256 超過
    char src[1024];
    build_nested(src, DEPTH, sizeof(src));
    Object *result = parse(src);
    // 深さ超過時は obj_nil (もしくは NULL) を返す現在仕様
    TEST_ASSERT_TRUE(result == obj_nil || result == NULL);
}

void test_parse_multiple_top_level(void) {
    const char *src = "(+ 1 2) 42 foo";
    Object *list = parse_all(src);
    TEST_ASSERT_NOT_NULL(list);
    // 1つ目: (+ 1 2)
    TEST_ASSERT_TRUE(is_cons(list));
    Object *expr1 = obj_car(list);
    TEST_ASSERT_TRUE(is_cons(expr1));
    TEST_ASSERT_TRUE(is_operator(obj_car(expr1)));
    TEST_ASSERT_EQUAL(OP_PLUS, obj_operator_type(obj_car(expr1)));
    // 2つ目: 42
    Object *rest2 = obj_cdr(list);
    TEST_ASSERT_TRUE(is_cons(rest2));
    Object *expr2 = obj_car(rest2);
    TEST_ASSERT_TRUE(is_number(expr2));
    TEST_ASSERT_EQUAL(42, obj_number_value(expr2));
    // 3つ目: foo
    Object *rest3 = obj_cdr(rest2);
    TEST_ASSERT_TRUE(is_cons(rest3));
    Object *expr3 = obj_car(rest3);
    TEST_ASSERT_TRUE(is_symbol(expr3));
    TEST_ASSERT_EQUAL_STRING("foo", obj_symbol_name(expr3));
    // その次は nil
    Object *rest4 = obj_cdr(rest3);
    TEST_ASSERT_TRUE(rest4 == obj_nil || (is_cons(rest4)==false && rest4==obj_nil));
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_parse_number);
    RUN_TEST(test_parse_symbol);
    RUN_TEST(test_parse_simple_list);
    RUN_TEST(test_parse_deep_nest_ok);
    RUN_TEST(test_parse_deep_nest_overflow);
    RUN_TEST(test_parse_multiple_top_level);

    return UNITY_END();
}
