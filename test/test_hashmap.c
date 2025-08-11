#include <unity.h>
#include "../src/hashmap.h"
#include <string.h>
#include <stdio.h>

static HashMap test_map;

void setUp(void) {
    hashmap_init(&test_map);
}

void tearDown(void) {
    // �n�b�V���}�b�v�̃N���[���A�b�v�͓��ɕs�v�i�ÓI�z��j
}

//------------------------------------------
// �n�b�V���}�b�v�̃e�X�g
//------------------------------------------

void test_hashmap_basic_set_get(void) {
    const char* key = "name";
    const char* value = "Alice";
    
    bool result = hashmap_set(&test_map, key, strlen(key), (void*)value);
    TEST_ASSERT_TRUE(result);
    
    char* retrieved = (char*)hashmap_get(&test_map, key, strlen(key));
    TEST_ASSERT_NOT_NULL(retrieved);
    TEST_ASSERT_EQUAL_STRING(value, retrieved);
}

void test_hashmap_multiple_entries(void) {
    const char* key1 = "age";
    const char* value1 = "25";
    const char* key2 = "city";
    const char* value2 = "Tokyo";
    
    // �����̃G���g����ݒ�
    TEST_ASSERT_TRUE(hashmap_set(&test_map, key1, strlen(key1), (void*)value1));
    TEST_ASSERT_TRUE(hashmap_set(&test_map, key2, strlen(key2), (void*)value2));
    
    // �����̃G���g�����擾
    char* retrieved1 = (char*)hashmap_get(&test_map, key1, strlen(key1));
    char* retrieved2 = (char*)hashmap_get(&test_map, key2, strlen(key2));
    
    TEST_ASSERT_EQUAL_STRING(value1, retrieved1);
    TEST_ASSERT_EQUAL_STRING(value2, retrieved2);
}

void test_hashmap_nonexistent_key(void) {
    const char* key = "nonexistent";
    
    void* result = hashmap_get(&test_map, key, strlen(key));
    TEST_ASSERT_NULL(result);
}

void test_hashmap_key_overwrite(void) {
    const char* key = "counter";
    const char* value1 = "1";
    const char* value2 = "2";
    
    // �ŏ��̒l��ݒ�
    TEST_ASSERT_TRUE(hashmap_set(&test_map, key, strlen(key), (void*)value1));
    char* retrieved1 = (char*)hashmap_get(&test_map, key, strlen(key));
    TEST_ASSERT_EQUAL_STRING(value1, retrieved1);
    
    // �l���㏑��
    TEST_ASSERT_TRUE(hashmap_set(&test_map, key, strlen(key), (void*)value2));
    char* retrieved2 = (char*)hashmap_get(&test_map, key, strlen(key));
    TEST_ASSERT_EQUAL_STRING(value2, retrieved2);
}

void test_hashmap_hash_collision(void) {
    const char* key1 = "key1";
    const char* value1 = "value1";
    const char* key2 = "key2";
    const char* value2 = "value2";
    
    // �قȂ�L�[��ݒ�i�n�b�V���Փ˂���������\���j
    TEST_ASSERT_TRUE(hashmap_set(&test_map, key1, strlen(key1), (void*)value1));
    TEST_ASSERT_TRUE(hashmap_set(&test_map, key2, strlen(key2), (void*)value2));
    
    // �����̃L�[�Ő������l���擾�ł��邱�Ƃ��m�F
    char* retrieved1 = (char*)hashmap_get(&test_map, key1, strlen(key1));
    char* retrieved2 = (char*)hashmap_get(&test_map, key2, strlen(key2));
    
    TEST_ASSERT_EQUAL_STRING(value1, retrieved1);
    TEST_ASSERT_EQUAL_STRING(value2, retrieved2);
}

void test_hashmap_empty_string_key(void) {
    const char* key = "";
    const char* value = "empty_key_value";
    
    TEST_ASSERT_TRUE(hashmap_set(&test_map, key, strlen(key), (void*)value));
    
    char* retrieved = (char*)hashmap_get(&test_map, key, strlen(key));
    TEST_ASSERT_EQUAL_STRING(value, retrieved);
}

void test_hashmap_large_number_of_entries(void) {
    char keys[100][20];
    char values[100][20];
    
    // �����̃G���g����ݒ�
    for (int i = 0; i < 100; i++) {
        sprintf(keys[i], "key_%d", i);
        sprintf(values[i], "value_%d", i);
        TEST_ASSERT_TRUE(hashmap_set(&test_map, keys[i], strlen(keys[i]), (void*)values[i]));
    }
    
    // �S�ẴG���g�����������擾�ł��邱�Ƃ��m�F
    for (int i = 0; i < 100; i++) {
        char* retrieved = (char*)hashmap_get(&test_map, keys[i], strlen(keys[i]));
        TEST_ASSERT_EQUAL_STRING(values[i], retrieved);
    }
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_hashmap_basic_set_get);
    RUN_TEST(test_hashmap_multiple_entries);
    RUN_TEST(test_hashmap_nonexistent_key);
    RUN_TEST(test_hashmap_key_overwrite);
    RUN_TEST(test_hashmap_hash_collision);
    RUN_TEST(test_hashmap_empty_string_key);
    RUN_TEST(test_hashmap_large_number_of_entries);
    
    return UNITY_END();
}
