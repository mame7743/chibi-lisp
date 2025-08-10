#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>  // printf�̂��߂ɒǉ�
#include "heap.h"
#define HEAP_SIZE 2048
#define CHUNK_SIZE 32
#define CHUNK_COUNT (HEAP_SIZE / CHUNK_SIZE)

static uint8_t heap[HEAP_SIZE];

// 0 = free, 0xFF = continuation, ���̓u���b�N��(chunks)
static uint8_t chunk_info[CHUNK_COUNT];

void heap_init(void) {
    int i;
    for (i = 0; i < CHUNK_COUNT; i++) {
        chunk_info[i] = 0;
    }
}

void *heap_alloc(size_t size) {
    int i, j;
    bool ok;

    // �T�C�Y0�̏ꍇ��NULL��Ԃ�
    if (size == 0) {
        return NULL;
    }

    int needed = (size + CHUNK_SIZE - 1) / CHUNK_SIZE;
    for (i = 0; i <= CHUNK_COUNT - needed; i++) {
        // �󂫊m�F
        ok = true;
        for (j = 0; j < needed; j++) {
            if (chunk_info[i + j] != 0) {
                ok = false;
                break;
            }
        }
        if (ok) {
            // �擪�ɃT�C�Y���L�^
            chunk_info[i] = needed;
            // �㑱�� continuation �}�[�N
            for (j = 1; j < needed; j++) {
                chunk_info[i + j] = 0xFF;
            }
            return &heap[i * CHUNK_SIZE];
        }
    }
    return NULL;  // �������s��
}

void heap_free(void *ptr) {
    int index     = ((uint8_t *)ptr - heap) / CHUNK_SIZE;
    uint8_t count = chunk_info[index];
    if (count == 0 || count == 0xFF) return;  // ����
    // �擪�ƌ㑱chunk��free��
    for (int i = 0; i < count; i++) {
        chunk_info[index + i] = 0;
    }
}

void heap_dump(void) {
    printf("Heap dump:\n");
    for (int i = 0; i < CHUNK_COUNT; i++) {
        if (chunk_info[i] == 0) {
            printf("Chunk %d: Free\n", i);
        } else if (chunk_info[i] == 0xFF) {
            printf("Chunk %d: Continuation\n", i);
        } else {
            printf("Chunk %d: Allocated (%d bytes)\n", i, chunk_info[i] * CHUNK_SIZE);
        }
    }
}