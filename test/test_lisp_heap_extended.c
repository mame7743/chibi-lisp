// test_lisp_heap_extended.c
#include "../lib/unity/src/unity.h"
#include <string.h>
#include "../src/lisp_heap.h"

// Extended test functions based on test_spec.yaml

void setUp(void) {
    // Initialize before each test
    lisp_heap_init();
}

void tearDown(void) {
    // Clean up after each test
}

void test_nil_value_reference() {
    TEST_ASSERT_NOT_NULL(nil);
    TEST_ASSERT_EQUAL(VALUE_NIL, nil->kind);
}

void test_boolean_value_reference() {
    TEST_ASSERT_NOT_NULL(true_value);
    TEST_ASSERT_EQUAL(VALUE_BOOL, true_value->kind);
    TEST_ASSERT_EQUAL(1, true_value->data.number);
    
    TEST_ASSERT_NOT_NULL(false_value);
    TEST_ASSERT_EQUAL(VALUE_BOOL, false_value->kind);
    TEST_ASSERT_EQUAL(0, false_value->data.number);
}

void test_value_type_check() {
    // Integer value test
    Value* int_val = make_int_value(123);
    TEST_ASSERT_NOT_NULL(int_val);
    TEST_ASSERT_EQUAL(VALUE_INT, int_val->kind);
    TEST_ASSERT_EQUAL(123, int_val->data.number);
    
    // String value test
    Value* str_val = make_string_value("test_string");
    TEST_ASSERT_NOT_NULL(str_val);
    TEST_ASSERT_EQUAL(VALUE_STRING, str_val->kind);
    TEST_ASSERT_EQUAL_STRING("test_string", str_val->data.string.text);
    TEST_ASSERT_EQUAL(11, str_val->data.string.len);
}

Value* allocate_all_values(int count) {
    Value* last = NULL;
    for (int i = 0; i < count; i++) {
        Value* v = lisp_heap_alloc();
        if (!v) return NULL;
        last = v;
    }
    return last;
}

void test_memory_exhaustion_error_handling() {
    // Allocate all available Values
    Value* last_valid = allocate_all_values(1024);  // LISP_HEAP_SIZE
    TEST_ASSERT_NOT_NULL(last_valid);
    
    // Additional allocations should fail
    Value* should_fail1 = make_int_value(42);
    TEST_ASSERT_NULL(should_fail1);
    
    Value* should_fail2 = make_string_value("test");
    TEST_ASSERT_NULL(should_fail2);
    
    Value* should_fail3 = make_symbol_value("symbol");
    TEST_ASSERT_NULL(should_fail3);
}

void test_string_deep_copy() {
    char original[] = "original";
    Value* str_val = make_string_value(original);
    TEST_ASSERT_NOT_NULL(str_val);
    TEST_ASSERT_EQUAL_STRING("original", str_val->data.string.text);
    
    // Modify the original string
    strcpy(original, "modified");
    
    // Value's string should remain unchanged
    TEST_ASSERT_EQUAL_STRING("original", str_val->data.string.text);
}

void test_symbol_uniqueness() {
    Value* sym1 = make_symbol_value("same_symbol");
    Value* sym2 = make_symbol_value("same_symbol");
    
    TEST_ASSERT_NOT_NULL(sym1);
    TEST_ASSERT_NOT_NULL(sym2);
    TEST_ASSERT_NOT_EQUAL(sym1, sym2);  // Different Value objects
    TEST_ASSERT_NOT_EQUAL(sym1->data.symbol, sym2->data.symbol);  // Different string pointers
    TEST_ASSERT_EQUAL_STRING(sym1->data.symbol, sym2->data.symbol);  // Same content
}

void test_nested_cons_creation() {
    Value* car = make_int_value(1);
    Value* cdr = make_int_value(2);
    Value* cons = make_cons_value(car, cdr);
    
    TEST_ASSERT_NOT_NULL(cons);
    TEST_ASSERT_EQUAL(VALUE_CONS, cons->kind);
    TEST_ASSERT_EQUAL_PTR(car, cons->data.cons.car);
    TEST_ASSERT_EQUAL_PTR(cdr, cons->data.cons.cdr);
    TEST_ASSERT_EQUAL(VALUE_INT, cons->data.cons.car->kind);
    TEST_ASSERT_EQUAL(VALUE_INT, cons->data.cons.cdr->kind);
    TEST_ASSERT_EQUAL(1, cons->data.cons.car->data.number);
    TEST_ASSERT_EQUAL(2, cons->data.cons.cdr->data.number);
    
    // Create nested cons
    Value* outer_cons = make_cons_value(cons, make_int_value(3));
    TEST_ASSERT_NOT_NULL(outer_cons);
    TEST_ASSERT_EQUAL(VALUE_CONS, outer_cons->kind);
    TEST_ASSERT_EQUAL_PTR(cons, outer_cons->data.cons.car);
    TEST_ASSERT_EQUAL(3, outer_cons->data.cons.cdr->data.number);
}

void test_value_reuse_after_free() {
    Value* values[5];
    
    // Allocate 5 values
    for (int i = 0; i < 5; i++) {
        values[i] = lisp_heap_alloc();
        TEST_ASSERT_NOT_NULL(values[i]);
    }
    
    // Free all values
    for (int i = 0; i < 5; i++) {
        lisp_heap_free(values[i]);
    }
    
    // Allocate again - should reuse the freed values
    for (int i = 0; i < 5; i++) {
        Value* reused = lisp_heap_alloc();
        TEST_ASSERT_NOT_NULL(reused);
        // The reused value should be one of the previously freed values
        bool found = false;
        for (int j = 0; j < 5; j++) {
            if (reused == values[j]) {
                found = true;
                break;
            }
        }
        TEST_ASSERT_TRUE(found);
    }
}

int main() {
    UNITY_BEGIN();
    
    RUN_TEST(test_nil_value_reference);
    RUN_TEST(test_boolean_value_reference);
    RUN_TEST(test_value_type_check);
    RUN_TEST(test_memory_exhaustion_error_handling);
    RUN_TEST(test_string_deep_copy);
    RUN_TEST(test_symbol_uniqueness);
    RUN_TEST(test_nested_cons_creation);
    RUN_TEST(test_value_reuse_after_free);
    
    return UNITY_END();
}
