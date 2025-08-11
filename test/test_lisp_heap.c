// test_lisp_heap.c
#include "../lib/unity/src/unity.h"
#include <string.h>
#include "../src/lisp_heap.h"

// Helper functions
Value* test_lisp_heap_alloc_many(int count) {
    Value* last = NULL;
    for (int i = 0; i < count; i++) {
        Value* v = lisp_heap_alloc();
        if (!v) return NULL;  // Return NULL when exhausted
        last = v;
    }
    return last;
}

void setUp(void) {
    // Initialize before each test
    lisp_heap_init();
}

void tearDown(void) {
    // Clean up after each test
}

// Test functions
void test_lisp_heap_init() {
    // Test passes if no crashes occur during initialization
    TEST_PASS();
}

void test_basic_value_allocation() {
    Value* v = lisp_heap_alloc();
    TEST_ASSERT_NOT_NULL(v);
}

void test_multiple_value_allocation() {
    Value* v1 = lisp_heap_alloc();
    Value* v2 = lisp_heap_alloc();
    Value* v3 = lisp_heap_alloc();
    
    TEST_ASSERT_NOT_NULL(v1);
    TEST_ASSERT_NOT_NULL(v2);
    TEST_ASSERT_NOT_NULL(v3);
    TEST_ASSERT_NOT_EQUAL(v1, v2);
    TEST_ASSERT_NOT_EQUAL(v2, v3);
    TEST_ASSERT_NOT_EQUAL(v1, v3);
}

void test_value_free_and_reuse() {
    Value* v1 = lisp_heap_alloc();
    TEST_ASSERT_NOT_NULL(v1);
    
    lisp_heap_free(v1);
    
    Value* v2 = lisp_heap_alloc();
    TEST_ASSERT_NOT_NULL(v2);
    // The freed Value should be reused
    TEST_ASSERT_EQUAL_PTR(v1, v2);
}

void test_heap_exhaustion() {
    // Try to allocate LISP_HEAP_SIZE + 1 Values
    Value* last_valid = test_lisp_heap_alloc_many(1024);  // LISP_HEAP_SIZE
    TEST_ASSERT_NOT_NULL(last_valid);
    
    // One more allocation should fail
    Value* should_fail = lisp_heap_alloc();
    TEST_ASSERT_NULL(should_fail);
}

void test_make_int_value() {
    Value* v = make_int_value(42);
    TEST_ASSERT_NOT_NULL(v);
    TEST_ASSERT_EQUAL(VALUE_INT, v->kind);
    TEST_ASSERT_EQUAL(42, v->data.number);
}

void test_make_string_value() {
    Value* v = make_string_value("hello");
    TEST_ASSERT_NOT_NULL(v);
    TEST_ASSERT_EQUAL(VALUE_STRING, v->kind);
    TEST_ASSERT_EQUAL(5, v->data.string.len);
    TEST_ASSERT_EQUAL_STRING("hello", v->data.string.text);
}

void test_make_symbol_value() {
    Value* v = make_symbol_value("symbol_name");
    TEST_ASSERT_NOT_NULL(v);
    TEST_ASSERT_EQUAL(VALUE_SYMBOL, v->kind);
    TEST_ASSERT_EQUAL_STRING("symbol_name", v->data.symbol);
}

void test_make_cons_value() {
    Value* car = make_int_value(1);
    Value* cdr = make_int_value(2);
    Value* cons = make_cons_value(car, cdr);
    
    TEST_ASSERT_NOT_NULL(cons);
    TEST_ASSERT_EQUAL(VALUE_CONS, cons->kind);
    TEST_ASSERT_EQUAL_PTR(car, cons->data.cons.car);
    TEST_ASSERT_EQUAL_PTR(cdr, cons->data.cons.cdr);
    TEST_ASSERT_EQUAL(1, cons->data.cons.car->data.number);
    TEST_ASSERT_EQUAL(2, cons->data.cons.cdr->data.number);
}

int main() {
    UNITY_BEGIN();
    
    RUN_TEST(test_lisp_heap_init);
    RUN_TEST(test_basic_value_allocation);
    RUN_TEST(test_multiple_value_allocation);
    RUN_TEST(test_value_free_and_reuse);
    RUN_TEST(test_heap_exhaustion);
    RUN_TEST(test_make_int_value);
    RUN_TEST(test_make_string_value);
    RUN_TEST(test_make_symbol_value);
    RUN_TEST(test_make_cons_value);
    
    return UNITY_END();
}
