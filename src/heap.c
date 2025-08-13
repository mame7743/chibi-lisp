#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>  // printfのために追加
#include "heap.h"

#define HEAP_SIZE 2048
#define CHUNK_SIZE 32
#define CHUNK_COUNT (HEAP_SIZE / CHUNK_SIZE)

static uint8_t heap[HEAP_SIZE];
static uint8_t allocation_bitmap[CHUNK_COUNT]; // 1 = allocated, 0 = free
static uint8_t size_info[CHUNK_COUNT]; // allocated blockのサイズ(chunks単位)

void heap_init(void) {
    memset(allocation_bitmap, 0, CHUNK_COUNT);
    memset(size_info, 0, CHUNK_COUNT);
}

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

void heap_free(void *ptr) {
    // NULLポインタチェック
    if (ptr == NULL) return;
    
    // ヒープ範囲内かチェック
    if ((uint8_t*)ptr < heap || (uint8_t*)ptr >= heap + HEAP_SIZE) {
        printf("WARNING: heap_free called with pointer outside heap range: %p\n", ptr);
        return;
    }
    
    int index = ((uint8_t *)ptr - heap) / CHUNK_SIZE;
    
    // インデックス範囲チェック
    if (index < 0 || index >= CHUNK_COUNT) {
        printf("WARNING: heap_free called with invalid index: %d\n", index);
        return;
    }
    
    // 割り当て済みかチェック
    if (allocation_bitmap[index] == 0) {
        printf("WARNING: heap_free called on already freed chunk: %d\n", index);
        return;
    }
    
    // サイズ情報を取得
    uint8_t count = size_info[index];
    if (count == 0) {
        printf("WARNING: heap_free called on chunk with no size info: %d\n", index);
        return;
    }
    
    // 割り当てを解除
    for (int i = 0; i < count; i++) {
        allocation_bitmap[index + i] = 0;
    }
    
    // サイズ情報をクリア
    size_info[index] = 0;
}

void heap_dump(void) {
    printf("Heap dump:\n");
    for (int i = 0; i < CHUNK_COUNT; i++) {
        if (allocation_bitmap[i] == 0) {
            printf("Chunk %d: Free\n", i);
        } else {
            if (size_info[i] > 0) {
                printf("Chunk %d: Allocated block start (%d chunks, %d bytes)\n", 
                       i, size_info[i], size_info[i] * CHUNK_SIZE);
            } else {
                printf("Chunk %d: Allocated (continuation)\n", i);
            }
        }
    }
}

// 統計関数の実装
size_t heap_total_size(void) {
    return HEAP_SIZE;
}

size_t heap_used_size(void) {
    size_t used = 0;
    for (int i = 0; i < CHUNK_COUNT; i++) {
        if (allocation_bitmap[i] == 1) {
            used += CHUNK_SIZE;
        }
    }
    return used;
}

size_t heap_free_size(void) {
    return heap_total_size() - heap_used_size();
}

size_t heap_allocated_chunks(void) {
    size_t allocated = 0;
    for (int i = 0; i < CHUNK_COUNT; i++) {
        if (allocation_bitmap[i] == 1) {
            allocated++;
        }
    }
    return allocated;
}

size_t heap_free_chunks(void) {
    size_t free_chunks = 0;
    for (int i = 0; i < CHUNK_COUNT; i++) {
        if (allocation_bitmap[i] == 0) {
            free_chunks++;
        }
    }
    return free_chunks;
}

// デバッグ用: ヒープの整合性をチェック
bool heap_validate(void) {
    bool valid = true;
    
    for (int i = 0; i < CHUNK_COUNT; i++) {
        if (allocation_bitmap[i] == 1 && size_info[i] > 0) {
            // ブロックの先頭チャンク
            int size = size_info[i];
            
            // ブロックのサイズが範囲内かチェック
            if (i + size > CHUNK_COUNT) {
                printf("ERROR: Block at %d extends beyond heap (size %d)\n", i, size);
                valid = false;
                continue;
            }
            
            // ブロック内の全チャンクが割り当て済みかチェック
            for (int j = 1; j < size; j++) {
                if (allocation_bitmap[i + j] != 1) {
                    printf("ERROR: Block at %d has unallocated chunk at %d\n", i, i + j);
                    valid = false;
                }
                if (size_info[i + j] != 0) {
                    printf("ERROR: Block continuation at %d has non-zero size info\n", i + j);
                    valid = false;
                }
            }
        } else if (allocation_bitmap[i] == 1 && size_info[i] == 0) {
            // 継続チャンク（size_info[i] == 0）
            // 前のチャンクがブロックの一部かチェック
            bool found_start = false;
            for (int j = i - 1; j >= 0; j--) {
                if (allocation_bitmap[j] == 1 && size_info[j] > 0) {
                    if (j + size_info[j] > i) {
                        found_start = true;
                        break;
                    }
                }
                if (allocation_bitmap[j] == 0) break;
            }
            if (!found_start) {
                printf("ERROR: Orphaned continuation chunk at %d\n", i);
                valid = false;
            }
        }
    }
    
    return valid;
}