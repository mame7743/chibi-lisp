// gc.c - Mark-and-sweep garbage collector
#include "gc.h"
#include "chibi_lisp.h"
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

    // GCマークスタック
    Object* stack[MAX_GC_MARK_STACK];
    size_t sp = 0;

    // プール内のオブジェクトかチェック
    if (!object_pool_is_valid(obj)) return;

    stack[sp++] = obj;

    while (sp > 0) {
        Object* current = stack[--sp];
        if (!current) continue;

        int index = object_pool_get_index(current);
        if (index < 0 || object_pool_is_marked(index)) continue;

        object_pool_set_mark(index);

        switch (current->type) {
            case OBJ_CONS:
                if (current->data.cons.car) stack[sp++] = current->data.cons.car;
                if (current->data.cons.cdr) stack[sp++] = current->data.cons.cdr;
                break;
            case OBJ_FUNCTION:
            case OBJ_LAMBDA:
                if (current->data.function.params) stack[sp++] = current->data.function.params;
                if (current->data.function.body) stack[sp++] = current->data.function.body;
                break;
            case OBJ_NIL:
            case OBJ_BOOL:
            case OBJ_NUMBER:
            case OBJ_STRING:
            case OBJ_SYMBOL:
            case OBJ_OPERATOR:
            case OBJ_BUILTIN:
            case OBJ_VOID:
                // プリミティブ型は子オブジェクトを持たない
                break;
        }
    }
}

//------------------------------------------
// ガベージコレクション実行
//------------------------------------------
void gc(void) {
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

// gc_collect関数のエイリアス（ヘッダーとの整合性のため）
void gc_collect(void) {
    gc();
}