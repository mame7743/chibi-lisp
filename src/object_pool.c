// object_pool.c - Simple object pool management

#include "object_pool.h"
#include "helper.h"
#include "heap.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//------------------------------------------
// プールデータ
//------------------------------------------
Object object_pool[OBJECT_POOL_SIZE];
uint8_t allocation_bitmap[BITMAP_SIZE];  // 使用状況をビットで管理
uint8_t marked_bitmap[BITMAP_SIZE];      // マークフラグをビットで管理

//------------------------------------------
// プール初期化
//------------------------------------------
void object_pool_init(void) {
    memset(object_pool, 0, sizeof(object_pool));
    memset(allocation_bitmap, 0, BITMAP_SIZE);
    memset(marked_bitmap, 0, BITMAP_SIZE);
}

//------------------------------------------
// オブジェクト確保・解放
//------------------------------------------
Object* object_pool_alloc(void) {
    for (int i = 0; i < OBJECT_POOL_SIZE; i++) {
        if (!bitmap_test(allocation_bitmap, i)) {
            bitmap_set(allocation_bitmap, i);
            memset(&object_pool[i], 0, sizeof(Object));
            return &object_pool[i];
        }
    }
    return NULL; // プール満杯
}

void object_pool_free(Object* obj) {
    if (!obj) return;

    // 可変長データの解放
    if (obj->type == OBJ_STRING && obj->data.string.text) {
        heap_free(obj->data.string.text);
        obj->data.string.text = NULL;
    }
    if (obj->type == OBJ_SYMBOL && obj->data.symbol.name) {
        heap_free(obj->data.symbol.name);
        obj->data.symbol.name = NULL;
    }

    int index = object_pool_get_index(obj);
    if (index >= 0) {
        bitmap_clear(allocation_bitmap, index);
        memset(obj, 0, sizeof(Object));
    }
}

//------------------------------------------
// プール情報取得
//------------------------------------------
int object_pool_get_index(Object* obj) {
    if (!obj || obj < object_pool || obj >= object_pool + OBJECT_POOL_SIZE) {
        return -1;
    }
    return (int)(obj - object_pool);
}

Object* object_pool_get_object(int index) {
    if (index < 0 || index >= OBJECT_POOL_SIZE) return NULL;
    return &object_pool[index];
}

//------------------------------------------
// プール状態管理
//------------------------------------------
bool object_pool_is_allocated(int index) {
    if (index < 0 || index >= OBJECT_POOL_SIZE) return false;
    return bitmap_test(allocation_bitmap, index);
}

size_t object_pool_used_count(void) {
    size_t count = 0;
    for (int i = 0; i < OBJECT_POOL_SIZE; i++) {
        if (bitmap_test(allocation_bitmap, i)) count++;
    }
    return count;
}

size_t object_pool_free_count(void) {
    return OBJECT_POOL_SIZE - object_pool_used_count();
}

bool object_pool_is_valid(Object* obj) {
    return object_pool_get_index(obj) >= 0;
}

//------------------------------------------
// GCマーク管理
//------------------------------------------
bool object_pool_is_marked(int index) {
    if (index < 0 || index >= OBJECT_POOL_SIZE) return false;
    return bitmap_test(marked_bitmap, index);
}

void object_pool_set_mark(int index) {
    if (index >= 0 && index < OBJECT_POOL_SIZE) {
        bitmap_set(marked_bitmap, index);
    }
}

void object_pool_clear_mark(int index) {
    if (index >= 0 && index < OBJECT_POOL_SIZE) {
        bitmap_clear(marked_bitmap, index);
    }
}

void object_pool_clear_all_marks(void) {
    memset(marked_bitmap, 0, BITMAP_SIZE);
}

//------------------------------------------
// デバッグ機能
//------------------------------------------
void object_pool_dump(void) {
    printf("Object Pool Dump:\n");
    size_t used = object_pool_used_count();
    printf("Used: %zu/%d objects\n", used, OBJECT_POOL_SIZE);

    for (int i = 0; i < OBJECT_POOL_SIZE && i < 20; i++) { // 最初の20個のみ表示
        if (bitmap_test(allocation_bitmap, i)) {
            printf("  [%d]: ", i);
            switch (object_pool[i].type) {
                case OBJ_NIL:
                    printf("NIL");
                    break;
                case OBJ_BOOL:
                    printf("BOOL(%s)", object_pool[i].data.number ? "true" : "false");
                    break;
                case OBJ_NUMBER:
                    printf("NUMBER(%d)", object_pool[i].data.number);
                    break;
                case OBJ_SYMBOL:
                    printf("SYMBOL(%s)", object_pool[i].data.symbol.name ? object_pool[i].data.symbol.name : "NULL");
                    break;
                case OBJ_STRING:
                    printf("STRING(%s)", object_pool[i].data.string.text ? object_pool[i].data.string.text : "NULL");
                    break;
                case OBJ_CONS:
                    printf("CONS");
                    break;
                case OBJ_FUNCTION:
                    printf("FUNCTION");
                    break;
                default:
                    printf("UNKNOWN(%d)", object_pool[i].type);
                    break;
            }
            if (bitmap_test(marked_bitmap, i)) printf(" [MARKED]");
            printf("\n");
        }
    }
    if (used > 20) {
        printf("  ... and %zu more\n", used - 20);
    }
}

void object_pool_dump_bitmap(void) {
    printf("Allocation Bitmap: ");
    for (int i = 0; i < BITMAP_SIZE; i++) {
        printf("%02x ", allocation_bitmap[i]);
    }
    printf("\nMarked Bitmap:     ");
    for (int i = 0; i < BITMAP_SIZE; i++) {
        printf("%02x ", marked_bitmap[i]);
    }
    printf("\n");
}