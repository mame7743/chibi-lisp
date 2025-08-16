// heap.c - Variable-length data heap management
#include "heap.h"
#include "chibi_lisp.h"
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

//------------------------------------------
// �q�[�v�ݒ�
//------------------------------------------
#define CHUNK_COUNT (HEAP_SIZE / CHUNK_SIZE)

//------------------------------------------
// �q�[�v�f�[�^
//------------------------------------------
static uint8_t heap[HEAP_SIZE];
static uint8_t allocation_bitmap[CHUNK_COUNT]; // 1 = allocated, 0 = free
static uint8_t size_info[CHUNK_COUNT]; // allocated block�̃T�C�Y(chunks�P��)

//------------------------------------------
// �q�[�v������
//------------------------------------------
void heap_init(void) {
    memset(allocation_bitmap, 0, CHUNK_COUNT);
    memset(size_info, 0, CHUNK_COUNT);
}

//------------------------------------------
// ���������蓖��
//------------------------------------------
void *heap_alloc(size_t size) {
    // �T�C�Y0�̏ꍇ��NULL��Ԃ�
    if (size == 0) {
        return NULL;
    }

    int needed = (size + CHUNK_SIZE - 1) / CHUNK_SIZE;

    // �A�������󂫃`�����N��T��
    for (int i = 0; i <= CHUNK_COUNT - needed; i++) {
        bool ok = true;

        // �K�v�Ȑ��̃`�����N���󂫂��`�F�b�N
        for (int j = 0; j < needed; j++) {
            if (allocation_bitmap[i + j] != 0) {
                ok = false;
                break;
            }
        }

        if (ok) {
            // ���������`�����N�����蓖��
            for (int j = 0; j < needed; j++) {
                allocation_bitmap[i + j] = 1;
            }
            // �擪�`�����N�ɃT�C�Y�����L�^
            size_info[i] = needed;

            return &heap[i * CHUNK_SIZE];
        }
    }

    return NULL;  // �������s��
}

//------------------------------------------
// ���������
//------------------------------------------
void heap_free(void *ptr) {
    // NULL�|�C���^�`�F�b�N
    if (ptr == NULL) return;

    // �q�[�v�͈͓����`�F�b�N
    uint8_t *byte_ptr = (uint8_t *)ptr;
    if (byte_ptr < heap || byte_ptr >= heap + HEAP_SIZE) {
        return; // �͈͊O
    }

    // �`�����N�C���f�b�N�X���v�Z
    int chunk_index = (byte_ptr - heap) / CHUNK_SIZE;
    if (chunk_index < 0 || chunk_index >= CHUNK_COUNT) {
        return; // �͈͊O
    }

    // ���蓖�Ă��Ă��邩�`�F�b�N
    if (allocation_bitmap[chunk_index] == 0) {
        return; // ���ɉ���ς�
    }

    // �T�C�Y�����擾
    int chunks_to_free = size_info[chunk_index];
    if (chunks_to_free <= 0 || chunks_to_free > CHUNK_COUNT) {
        chunks_to_free = 1; // ���S��Ƃ��čŒ�1�`�����N���
    }

    // �`�����N�����
    for (int i = 0; i < chunks_to_free && (chunk_index + i) < CHUNK_COUNT; i++) {
        allocation_bitmap[chunk_index + i] = 0;
        size_info[chunk_index + i] = 0;
    }
}

//------------------------------------------
// �q�[�v���v�擾
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
// �f�o�b�O�@�\
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

    // ���蓖�Ă�ꂽ�u���b�N�̏ڍ�
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
// �q�[�v���؋@�\
//------------------------------------------
bool heap_validate(void) {
    for (int i = 0; i < CHUNK_COUNT; i++) {
        if (allocation_bitmap[i] != 0) {
            // ���蓖�čς݃`�����N�̏ꍇ�A�T�C�Y��񂪑Ó����`�F�b�N
            if (size_info[i] > 0) {
                // �擪�`�����N�̏ꍇ�A�A������`�����N���`�F�b�N
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