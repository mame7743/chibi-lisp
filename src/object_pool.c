#include "object_pool.h"
#include "heap.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "helper.h"

//------------------------------------------
// プールデータ
//------------------------------------------
Object object_pool[OBJECT_POOL_SIZE];
uint8_t allocation_bitmap[BITMAP_SIZE];  // 使用状況をビットで管理
uint8_t marked_bitmap[BITMAP_SIZE];  // マークフラグをビットで管理
static size_t used_count = 0;

//------------------------------------------
// ビットマップ操作
//------------------------------------------
bool bitmap_get_bit(uint8_t* bitmap, int index) {
    if (index < 0 || index >= OBJECT_POOL_SIZE) return false;
    int byte_index = index / 8;
    int bit_index = index % 8;
    return (bitmap[byte_index] & (1 << bit_index)) != 0;
}

void bitmap_set_bit(uint8_t* bitmap, int index) {
    if (index < 0 || index >= OBJECT_POOL_SIZE) return;
    int byte_index = index / 8;
    int bit_index = index % 8;
    bitmap[byte_index] |= (1 << bit_index);
}

void bitmap_clear_bit(uint8_t* bitmap, int index) {
    if (index < 0 || index >= OBJECT_POOL_SIZE) return;
    int byte_index = index / 8;
    int bit_index = index % 8;
    bitmap[byte_index] &= ~(1 << bit_index);
}

//------------------------------------------
// プール初期化
//------------------------------------------
void object_pool_init(void) {
    // ビットマップをクリア
    memset(allocation_bitmap, 0, BITMAP_SIZE);
    memset(marked_bitmap, 0, BITMAP_SIZE);

    // オブジェクトプールを初期化
    memset(object_pool, 0, sizeof(object_pool));

    used_count = 0;
}

//------------------------------------------
// オブジェクト確保・解放
//------------------------------------------
Object* object_pool_alloc(void) {
    const int bit_size = sizeof(allocation_bitmap[0]) * 8;
    // 空きスロットを検索
    for (int i = 0; i < OBJECT_POOL_SIZE; i++) {
        if (!bitmap_get_bit(allocation_bitmap, i)) {
            // 空きスロットを発見
            bitmap_set_bit(allocation_bitmap, i);
            used_count++;
            Object* obj = &object_pool[i];
            memset(obj, 0, sizeof(Object));
            bitmap_clear_bit(marked_bitmap, i);
            return obj;
        }
    }

    return NULL;  // メモリ不足
}

void object_pool_free(Object* obj) {
    if (!obj) return;  // NULLチェック

    int index = object_pool_get_index(obj);
    if (index < 0 || !bitmap_get_bit(allocation_bitmap, index)) {
        return;  // 無効なオブジェクトまたは既に解放済み
    }

    // 文字列やシンボルのメモリを解放（NULLポインタをセット）
    if (obj->type == OBJ_STRING && obj->data.string.text) {
        heap_free(obj->data.string.text);
        obj->data.string.text = NULL;  // 二重解放を防ぐ
    }
    if (obj->type == OBJ_SYMBOL && obj->data.symbol.name) {
        heap_free(obj->data.symbol.name);
        obj->data.symbol.name = NULL;  // 二重解放を防ぐ
    }

    // ビットマップをクリア
    bitmap_clear_bit(allocation_bitmap, index);
    bitmap_clear_bit(marked_bitmap, index);
    used_count--;

    // オブジェクトをクリア
    memset(obj, 0, sizeof(Object));
}

//------------------------------------------
// インデックス操作
//------------------------------------------
int object_pool_get_index(Object* obj) {
    if (!obj) return -1;

    ptrdiff_t diff = obj - object_pool;
    if (diff < 0 || diff >= OBJECT_POOL_SIZE) {
        return -1;  // プール外のオブジェクト
    }

    return (int)diff;
}

Object* object_pool_get_object(int index) {
    if (index < 0 || index >= OBJECT_POOL_SIZE) {
        return NULL;
    }

    return &object_pool[index];
}

//------------------------------------------
// 状態確認
//------------------------------------------
bool object_pool_is_allocated(int index) {
    return bitmap_get_bit(allocation_bitmap, index);
}

size_t object_pool_used_count(void) {
    return used_count;
}

size_t object_pool_free_count(void) {
    return OBJECT_POOL_SIZE - used_count;
}

bool object_pool_is_valid(Object* obj) {
    int index = object_pool_get_index(obj);
    return index >= 0 && bitmap_get_bit(allocation_bitmap, index);
}

//------------------------------------------
// GCマーク操作
//------------------------------------------
bool object_pool_is_marked(int index) {
    return bitmap_get_bit(marked_bitmap, index);
}

void object_pool_set_mark(int index) {
    bitmap_set_bit(marked_bitmap, index);
}

void object_pool_clear_mark(int index) {
    bitmap_clear_bit(marked_bitmap, index);
}

void object_pool_clear_all_marks(void) {
    memset(marked_bitmap, 0, BITMAP_SIZE);
}

//------------------------------------------
// デバッグ用関数
//------------------------------------------
void object_pool_dump(void) {
    printf("Object Pool Status:\n");
    printf("  Total Objects: %d\n", OBJECT_POOL_SIZE);
    printf("  Used: %zu\n", used_count);
    printf("  Free: %zu\n", OBJECT_POOL_SIZE - used_count);
    printf("  Usage: %.1f%%\n", (double)used_count / OBJECT_POOL_SIZE * 100.0);
    printf("  Bitmap Size: %d bytes\n", BITMAP_SIZE);
}

void object_pool_dump_bitmap(void) {
    printf("Allocation Bitmap:\n");
    for (int i = 0; i < OBJECT_POOL_SIZE; i++) {
        if (i % 64 == 0) printf("\n%04d: ", i);
        printf("%c", bitmap_get_bit(allocation_bitmap, i) ? '1' : '0');
        if ((i + 1) % 8 == 0) printf(" ");
    }
    printf("\n");
}
