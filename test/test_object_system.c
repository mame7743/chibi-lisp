// test_object_system.c
#include "../lib/unity/src/unity.h"
#include "../src/object.h"
#include "../src/object_pool.h"
#include "../src/gc.h"
#include "../src/helper.h" // bitmap_* ラッパー

// テスト用のビットマップヘルパー
// object_pool.cの内部データにアクセスするための外部宣言
extern uint8_t allocation_bitmap[];
extern uint8_t marked_bitmap[];
#define BITMAP_SIZE ((OBJECT_POOL_SIZE + 7) / 8)

void setUp(void) {
    object_system_init();  // 固定オブジェクトも含めて初期化
}

void tearDown(void) {
    object_system_cleanup();  // クリーンアップ
}

void test_bitmap_operations() {
    // ビットマップ操作のテスト
    TEST_ASSERT_FALSE(bitmap_get_bit(allocation_bitmap, 0));
    TEST_ASSERT_FALSE(bitmap_get_bit(allocation_bitmap, 100));

    bitmap_set_bit(allocation_bitmap, 0);
    bitmap_set_bit(allocation_bitmap, 100);

    TEST_ASSERT_TRUE(bitmap_get_bit(allocation_bitmap, 0));
    TEST_ASSERT_TRUE(bitmap_get_bit(allocation_bitmap, 100));
    TEST_ASSERT_FALSE(bitmap_get_bit(allocation_bitmap, 50));

    bitmap_clear_bit(allocation_bitmap, 0);
    TEST_ASSERT_FALSE(bitmap_get_bit(allocation_bitmap, 0));
    TEST_ASSERT_TRUE(bitmap_get_bit(allocation_bitmap, 100));
}

void test_object_pool_allocation() {
    size_t initial_free = object_pool_free_count();

    Object* obj1 = object_pool_alloc();
    Object* obj2 = object_pool_alloc();

    TEST_ASSERT_NOT_NULL(obj1);
    TEST_ASSERT_NOT_NULL(obj2);
    TEST_ASSERT_NOT_EQUAL(obj1, obj2);

    TEST_ASSERT_EQUAL(initial_free - 2, object_pool_free_count());
    TEST_ASSERT_EQUAL(2, object_pool_used_count());

    // インデックス確認
    int idx1 = object_pool_get_index(obj1);
    int idx2 = object_pool_get_index(obj2);

    TEST_ASSERT_TRUE(idx1 >= 0 && idx1 < OBJECT_POOL_SIZE);
    TEST_ASSERT_TRUE(idx2 >= 0 && idx2 < OBJECT_POOL_SIZE);
    TEST_ASSERT_NOT_EQUAL(idx1, idx2);

    TEST_ASSERT_TRUE(object_pool_is_allocated(idx1));
    TEST_ASSERT_TRUE(object_pool_is_allocated(idx2));
    TEST_ASSERT_TRUE(object_pool_is_valid(obj1));
    TEST_ASSERT_TRUE(object_pool_is_valid(obj2));
}

void test_object_pool_free() {
    Object* obj = object_pool_alloc();
    TEST_ASSERT_NOT_NULL(obj);

    int index = object_pool_get_index(obj);
    TEST_ASSERT_TRUE(object_pool_is_allocated(index));

    size_t used_before = object_pool_used_count();
    object_pool_free(obj);

    TEST_ASSERT_FALSE(object_pool_is_allocated(index));
    TEST_ASSERT_EQUAL(used_before - 1, object_pool_used_count());
}

void test_make_number() {
    extern Object* make_number(int value);

    Object* num = make_number(42);
    TEST_ASSERT_NOT_NULL(num);
    TEST_ASSERT_EQUAL(OBJ_NUMBER, num->type);
    TEST_ASSERT_EQUAL(42, num->data.number);
}

void test_gc_mark_and_sweep() {
    extern Object* make_number(int value);
    extern Object* make_cons(Object* car, Object* cdr);

    // オブジェクトを作成
    Object* num1 = make_number(1);
    Object* num2 = make_number(2);
    Object* cons = make_cons(num1, num2);
    Object* orphan = make_number(999);  // 到達不可能

    TEST_ASSERT_NOT_NULL(num1);
    TEST_ASSERT_NOT_NULL(num2);
    TEST_ASSERT_NOT_NULL(cons);
    TEST_ASSERT_NOT_NULL(orphan);

    size_t used_before = object_pool_used_count();
    TEST_ASSERT_EQUAL(4, used_before);

    // consをルートに追加
    gc_add_root(&cons);

    // GC実行
    gc_collect();

    // 到達可能なオブジェクトは生存
    TEST_ASSERT_TRUE(object_pool_is_valid(cons));
    TEST_ASSERT_TRUE(object_pool_is_valid(num1));
    TEST_ASSERT_TRUE(object_pool_is_valid(num2));

    // 統計確認
    TEST_ASSERT_EQUAL(1, gc_total_collections());
    TEST_ASSERT_EQUAL(1, gc_last_collected_count());  // orphanが回収される

    gc_remove_root(&cons);
}

void test_gc_circular_reference() {
    extern Object* make_cons(Object* car, Object* cdr);
    extern Object* make_number(int value);

    // 循環参照を作成
    Object* cons1 = object_pool_alloc();
    Object* cons2 = object_pool_alloc();
    Object* num = make_number(42);

    cons1->type = OBJ_CONS;
    cons1->data.cons.car = num;
    cons1->data.cons.cdr = cons2;

    cons2->type = OBJ_CONS;
    cons2->data.cons.car = cons1;  // 循環参照
    cons2->data.cons.cdr = NULL;

    // cons1をルートに登録
    gc_add_root(&cons1);

    size_t used_before = object_pool_used_count();

    // GC実行 - 循環参照でも正しく処理されるはず
    gc_collect();

    // 循環参照構造は到達可能なので回収されない
    TEST_ASSERT_TRUE(object_pool_is_valid(cons1));
    TEST_ASSERT_TRUE(object_pool_is_valid(cons2));
    TEST_ASSERT_TRUE(object_pool_is_valid(num));

    gc_remove_root(&cons1);
}

void test_object_pool_exhaustion() {
    // プールを満杯にする
    Object* objects[OBJECT_POOL_SIZE];

    for (int i = 0; i < OBJECT_POOL_SIZE; i++) {
        objects[i] = object_pool_alloc();
        if (!objects[i]) break;
    }

    TEST_ASSERT_EQUAL(0, object_pool_free_count());

    // 追加の確保は失敗するはず
    Object* should_fail = object_pool_alloc();
    TEST_ASSERT_NULL(should_fail);
}

void test_fixed_objects() {
    // 固定オブジェクトが正しく初期化されているかテスト
    TEST_ASSERT_NOT_NULL(obj_nil);
    TEST_ASSERT_NOT_NULL(obj_true);
    TEST_ASSERT_NOT_NULL(obj_false);

    TEST_ASSERT_EQUAL(OBJ_NIL, obj_nil->type);
    TEST_ASSERT_EQUAL(OBJ_BOOL, obj_true->type);
    TEST_ASSERT_EQUAL(OBJ_BOOL, obj_false->type);

    // 固定オブジェクトはobject_pool外にある
    TEST_ASSERT_FALSE(object_pool_is_valid(obj_nil));
    TEST_ASSERT_FALSE(object_pool_is_valid(obj_true));
    TEST_ASSERT_FALSE(object_pool_is_valid(obj_false));

    // 型チェック関数のテスト
    TEST_ASSERT_TRUE(is_nil(obj_nil));
    TEST_ASSERT_FALSE(is_nil(obj_true));
    TEST_ASSERT_FALSE(is_nil(obj_false));
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_bitmap_operations);
    RUN_TEST(test_object_pool_allocation);
    RUN_TEST(test_object_pool_free);
    RUN_TEST(test_make_number);
    RUN_TEST(test_gc_mark_and_sweep);
    RUN_TEST(test_gc_circular_reference);
    RUN_TEST(test_object_pool_exhaustion);
    RUN_TEST(test_fixed_objects);

    return UNITY_END();
}
