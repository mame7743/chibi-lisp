// heap.c - Variable-length data heap management
#include "heap.h"
#include "chibi_lisp.h"
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

//------------------------------------------
// ヒープ設定
//------------------------------------------
#define CHUNK_COUNT (HEAP_SIZE / CHUNK_SIZE)

//------------------------------------------
// ヒープデータ
//------------------------------------------
static uint8_t heap[HEAP_SIZE];
static uint8_t allocation_bitmap[CHUNK_COUNT]; // 1 = allocated, 0 = free
static uint8_t size_info[CHUNK_COUNT]; // allocated blockのサイズ(chunks単位)

//------------------------------------------
// ヒープ初期化
//------------------------------------------
void heap_init(void) {
    memset(allocation_bitmap, 0, CHUNK_COUNT);
    memset(size_info, 0, CHUNK_COUNT);
}

//------------------------------------------
// メモリ割り当て
//------------------------------------------
void *heap_alloc(size_t size) {
    // サイズ0の場合はNULLを返す
    if (size == 0) {
        return NULL;
    }

    int needed = (size + CHUNK_SIZE - 1) / CHUNK_SIZE;

    // 連続した空きチャンクを探す
    for (int i = 0; i <= CHUNK_COUNT - needed; i++) {
        bool ok = true;

        // 必要な数のチャンクが空きかチェック
        for (int j = 0; j < needed; j++) {
            if (allocation_bitmap[i + j] != 0) {
                ok = false;
                break;
            }
        }

        if (ok) {
            // 見つかったチャンクを割り当て
            for (int j = 0; j < needed; j++) {
                allocation_bitmap[i + j] = 1;
            }
            // 先頭チャンクにサイズ情報を記録
            size_info[i] = needed;

            return &heap[i * CHUNK_SIZE];
        }
    }

    return NULL;  // メモリ不足
}

//------------------------------------------
// メモリ解放
//------------------------------------------
void heap_free(void *ptr) {
    // NULLポインタチェック
    if (ptr == NULL) return;

    // ヒープ範囲内かチェック
    uint8_t *byte_ptr = (uint8_t *)ptr;
    if (byte_ptr < heap || byte_ptr >= heap + HEAP_SIZE) {
        return; // 範囲外
    }

    // チャンクインデックスを計算
    int chunk_index = (byte_ptr - heap) / CHUNK_SIZE;
    if (chunk_index < 0 || chunk_index >= CHUNK_COUNT) {
        return; // 範囲外
    }

    // 割り当てられているかチェック
    if (allocation_bitmap[chunk_index] == 0) {
        return; // 既に解放済み
    }

    // サイズ情報を取得
    int chunks_to_free = size_info[chunk_index];
    if (chunks_to_free <= 0 || chunks_to_free > CHUNK_COUNT) {
        chunks_to_free = 1; // 安全策として最低1チャンク解放
    }

    // チャンクを解放
    for (int i = 0; i < chunks_to_free && (chunk_index + i) < CHUNK_COUNT; i++) {
        allocation_bitmap[chunk_index + i] = 0;
        size_info[chunk_index + i] = 0;
    }
}

//------------------------------------------
// ヒープ統計取得
//------------------------------------------
size_t heap_total_size(void) {
    return HEAP_SIZE;
}

size_t heap_used_size(void) {
    size_t used_chunks = 0;
    for (int i = 0; i < CHUNK_COUNT; i++) {
        if (allocation_bitmap[i] != 0) {
            used_chunks++;
        }
    }
    return used_chunks * CHUNK_SIZE;
}

size_t heap_free_size(void) {
    return heap_total_size() - heap_used_size();
}

size_t heap_allocated_chunks(void) {
    size_t count = 0;
    for (int i = 0; i < CHUNK_COUNT; i++) {
        if (allocation_bitmap[i] != 0 && size_info[i] > 0) {
            count++;
        }
    }
    return count;
}

size_t heap_free_chunks(void) {
    size_t used_chunks = 0;
    for (int i = 0; i < CHUNK_COUNT; i++) {
        if (allocation_bitmap[i] != 0) {
            used_chunks++;
        }
    }
    return CHUNK_COUNT - used_chunks;
}

//------------------------------------------
// デバッグ機能
//------------------------------------------
void heap_dump(void) {
    printf("Heap Dump:\n");
    printf("  Total size: %zu bytes (%d chunks)\n", (size_t)HEAP_SIZE, CHUNK_COUNT);
    printf("  Used size:  %zu bytes\n", heap_used_size());
    printf("  Free size:  %zu bytes\n", heap_free_size());
    printf("  Chunk size: %d bytes\n", CHUNK_SIZE);

    printf("Allocation bitmap: ");
    for (int i = 0; i < CHUNK_COUNT; i++) {
        printf("%d", allocation_bitmap[i] ? 1 : 0);
        if ((i + 1) % 8 == 0) printf(" ");
    }
    printf("\n");

    printf("Size info: ");
    for (int i = 0; i < CHUNK_COUNT; i++) {
        printf("%2d ", size_info[i]);
        if ((i + 1) % 8 == 0) printf("\n           ");
    }
    printf("\n");

    // 割り当てられたブロックの詳細
    printf("Allocated blocks:\n");
    for (int i = 0; i < CHUNK_COUNT; i++) {
        if (allocation_bitmap[i] != 0 && size_info[i] > 0) {
            printf("  Block at chunk %d: %d chunks (%d bytes)\n",
                   i, size_info[i], size_info[i] * CHUNK_SIZE);
        }
    }
}

void heap_clear(void) {
    memset(allocation_bitmap, 0, CHUNK_COUNT);
    memset(size_info, 0, CHUNK_COUNT);
}

//------------------------------------------
// ヒープ検証機能
//------------------------------------------
bool heap_validate(void) {
    for (int i = 0; i < CHUNK_COUNT; i++) {
        if (allocation_bitmap[i] != 0) {
            // 割り当て済みチャンクの場合、サイズ情報が妥当かチェック
            if (size_info[i] > 0) {
                // 先頭チャンクの場合、連続するチャンクもチェック
                for (int j = 1; j < size_info[i] && (i + j) < CHUNK_COUNT; j++) {
                    if (allocation_bitmap[i + j] == 0) {
                        printf("Heap validation error: chunk %d should be allocated\n", i + j);
                        return false;
                    }
                }
            }
        }
    }
    return true;
}