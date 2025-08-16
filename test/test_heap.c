#include <unity.h>
#include "../src/heap.h"
#include <stdio.h>

void setUp(void) {
    heap_init();
}

void tearDown(void) {
    // �e�e�X�g��̃N���[���A�b�v�͕s�v�iheap_init()�ŏ������j
}

//------------------------------------------
// �q�[�v�̃e�X�g
//------------------------------------------

void test_heap_basic_allocation(void) {
    void* ptr = heap_alloc(100);
    TEST_ASSERT_NOT_NULL(ptr);
}

void test_heap_multiple_allocations(void) {
    void* ptr1 = heap_alloc(50);
    void* ptr2 = heap_alloc(100);
    void* ptr3 = heap_alloc(200);

    TEST_ASSERT_NOT_NULL(ptr1);
    TEST_ASSERT_NOT_NULL(ptr2);
    TEST_ASSERT_NOT_NULL(ptr3);

    // �|�C���^���d�����Ă��Ȃ����Ƃ��m�F
    TEST_ASSERT_NOT_EQUAL(ptr1, ptr2);
    TEST_ASSERT_NOT_EQUAL(ptr2, ptr3);
    TEST_ASSERT_NOT_EQUAL(ptr1, ptr3);
}

void test_heap_free_and_realloc(void) {
    void* ptr1 = heap_alloc(100);
    TEST_ASSERT_NOT_NULL(ptr1);

    heap_free(ptr1);

    void* ptr2 = heap_alloc(100);
    TEST_ASSERT_NOT_NULL(ptr2);
    // �����̈悪�ė��p�����\��������
}

void test_heap_zero_size_allocation(void) {
    void* ptr = heap_alloc(0);
    TEST_ASSERT_NULL(ptr);
}

void test_heap_large_allocation(void) {
    // �q�[�v�S�̃T�C�Y�𒴂��銄�蓖�Ă𓮓I�Ɍv�Z
    size_t too_large = heap_total_size() + 1;
    void* ptr = heap_alloc(too_large);
    TEST_ASSERT_NULL(ptr);
}

void test_heap_fragmentation(void) {
    void* ptrs[10];
    int i;

    // �����̏����ȃu���b�N���m��
    for (i = 0; i < 10; i++) {
        ptrs[i] = heap_alloc(32);
        TEST_ASSERT_NOT_NULL(ptrs[i]);
    }

    // ������ɉ��
    for (i = 0; i < 10; i += 2) {
        heap_free(ptrs[i]);
    }

    // �����ȃu���b�N�͊m�ۂł���͂�
    void* small_ptr = heap_alloc(32);
    TEST_ASSERT_NOT_NULL(small_ptr);

    // �c������
    for (i = 1; i < 10; i += 2) {
        heap_free(ptrs[i]);
    }
    heap_free(small_ptr);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_heap_basic_allocation);
    RUN_TEST(test_heap_multiple_allocations);
    RUN_TEST(test_heap_free_and_realloc);
    RUN_TEST(test_heap_zero_size_allocation);
    RUN_TEST(test_heap_large_allocation);
    RUN_TEST(test_heap_fragmentation);

    return UNITY_END();
}
