#include "gc.h"
#include <stdio.h>
#include <string.h>

//------------------------------------------
// GCデータ
//------------------------------------------
static Object** gc_roots[MAX_ROOTS];
static size_t gc_root_count = 0;
static size_t gc_collections = 0;
static size_t gc_last_collected = 0;
static size_t gc_total_collected = 0;

//------------------------------------------
// 外部参照（固定オブジェクト）
//------------------------------------------
extern Object* obj_nil;
extern Object* obj_true;
extern Object* obj_false;

//------------------------------------------
// GC初期化
//------------------------------------------
void gc_init(void) {
    gc_root_count = 0;
    gc_collections = 0;
    gc_last_collected = 0;
    gc_total_collected = 0;
    memset(gc_roots, 0, sizeof(gc_roots));
}

//------------------------------------------
// ルートセット管理
//------------------------------------------
void gc_add_root(Object** root) {
    if (gc_root_count < MAX_ROOTS && root != NULL) {
        gc_roots[gc_root_count++] = root;
    }
}

void gc_remove_root(Object** root) {
    for (size_t i = 0; i < gc_root_count; i++) {
        if (gc_roots[i] == root) {
            // 最後の要素を現在の位置に移動
            gc_roots[i] = gc_roots[--gc_root_count];
            gc_roots[gc_root_count] = NULL;
            break;
        }
    }
}

//------------------------------------------
// マーク処理
//------------------------------------------
static void gc_mark_object(Object* obj) {
    if (!obj) return;

    // プール内のオブジェクトかチェック
    if (!object_pool_is_valid(obj)) return;

    int index = object_pool_get_index(obj);
    if (index < 0 || object_pool_is_marked(index)) return;

    // ビットマップでマーク
    object_pool_set_mark(index);

    // 子オブジェクトを再帰的にマーク
    switch (obj->type) {
        case OBJ_CONS:
            gc_mark_object(obj->data.cons.car);
            gc_mark_object(obj->data.cons.cdr);
            break;
        case OBJ_FUNCTION:
        case OBJ_LAMBDA:
            gc_mark_object(obj->data.function.params);
            gc_mark_object(obj->data.function.body);
            break;
        case OBJ_NIL:
        case OBJ_BOOL:
        case OBJ_NUMBER:
        case OBJ_STRING:
        case OBJ_SYMBOL:
            // プリミティブ型は子オブジェクトを持たない
            break;
    }
}

//------------------------------------------
// ガベージコレクション実行
//------------------------------------------
void gc_collect(void) {
    gc_collections++;
    gc_last_collected = 0;

    // マークフェーズ: 全オブジェクトのマークをクリア
    object_pool_clear_all_marks();

    // ルートオブジェクトからマーク
    for (size_t i = 0; i < gc_root_count; i++) {
        if (gc_roots[i] && *gc_roots[i]) {
            gc_mark_object(*gc_roots[i]);
        }
    }

    // 固定オブジェクトは常に生存（object_pool外なので何もしない）
    // obj_nil, obj_true, obj_falseはstatic領域にあるため、
    // ビットマップでのマークは不要

    // スイープフェーズ: マークされていないオブジェクトを回収
    for (int i = 0; i < OBJECT_POOL_SIZE; i++) {
        if (object_pool_is_allocated(i)) {
            Object* obj = object_pool_get_object(i);

            // 固定オブジェクトはobject_pool外なので、ここには現れない
            if (!object_pool_is_marked(i)) {
                object_pool_free(obj);
                gc_last_collected++;
            }
        }
    }

    gc_total_collected += gc_last_collected;
}

//------------------------------------------
// GC統計
//------------------------------------------
size_t gc_total_collections(void) {
    return gc_collections;
}

size_t gc_last_collected_count(void) {
    return gc_last_collected;
}

size_t gc_total_collected_count(void) {
    return gc_total_collected;
}

//------------------------------------------
// デバッグ用関数
//------------------------------------------
void gc_dump_roots(void) {
    printf("GC Root Objects:\n");
    for (size_t i = 0; i < gc_root_count; i++) {
        printf("  Root %zu: %p -> ", i, (void*)gc_roots[i]);
        if (gc_roots[i] && *gc_roots[i]) {
            extern void object_dump(Object* obj); // Object.cから参照
            object_dump(*gc_roots[i]);
        } else {
            printf("NULL");
        }
        printf("\n");
    }
}

void gc_dump_stats(void) {
    printf("GC Statistics:\n");
    printf("  Total Collections: %zu\n", gc_collections);
    printf("  Last Collected: %zu objects\n", gc_last_collected);
    printf("  Total Collected: %zu objects\n", gc_total_collected);
    printf("  Root Count: %zu/%d\n", gc_root_count, MAX_ROOTS);
}