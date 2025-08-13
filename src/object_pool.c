#include "object_pool.h"
#include "heap.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "helper.h"

//------------------------------------------
// �v�[���f�[�^
//------------------------------------------
Object object_pool[OBJECT_POOL_SIZE];
uint8_t allocation_bitmap[BITMAP_SIZE];  // �g�p�󋵂��r�b�g�ŊǗ�
uint8_t marked_bitmap[BITMAP_SIZE];  // �}�[�N�t���O���r�b�g�ŊǗ�
static size_t used_count = 0;

//------------------------------------------
// �r�b�g�}�b�v����
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
// �v�[��������
//------------------------------------------
void object_pool_init(void) {
    // �r�b�g�}�b�v���N���A
    memset(allocation_bitmap, 0, BITMAP_SIZE);
    memset(marked_bitmap, 0, BITMAP_SIZE);

    // �I�u�W�F�N�g�v�[����������
    memset(object_pool, 0, sizeof(object_pool));

    used_count = 0;
}

//------------------------------------------
// �I�u�W�F�N�g�m�ہE���
//------------------------------------------
Object* object_pool_alloc(void) {
    const int bit_size = sizeof(allocation_bitmap[0]) * 8;
    // �󂫃X���b�g������
    for (int i = 0; i < OBJECT_POOL_SIZE; i++) {
        if (!bitmap_get_bit(allocation_bitmap, i)) {
            // �󂫃X���b�g�𔭌�
            bitmap_set_bit(allocation_bitmap, i);
            used_count++;
            Object* obj = &object_pool[i];
            memset(obj, 0, sizeof(Object));
            bitmap_clear_bit(marked_bitmap, i);
            return obj;
        }
    }

    return NULL;  // �������s��
}

void object_pool_free(Object* obj) {
    if (!obj) return;  // NULL�`�F�b�N

    int index = object_pool_get_index(obj);
    if (index < 0 || !bitmap_get_bit(allocation_bitmap, index)) {
        return;  // �����ȃI�u�W�F�N�g�܂��͊��ɉ���ς�
    }

    // �������V���{���̃�����������iNULL�|�C���^���Z�b�g�j
    if (obj->type == OBJ_STRING && obj->data.string.text) {
        heap_free(obj->data.string.text);
        obj->data.string.text = NULL;  // ��d�����h��
    }
    if (obj->type == OBJ_SYMBOL && obj->data.symbol.name) {
        heap_free(obj->data.symbol.name);
        obj->data.symbol.name = NULL;  // ��d�����h��
    }

    // �r�b�g�}�b�v���N���A
    bitmap_clear_bit(allocation_bitmap, index);
    bitmap_clear_bit(marked_bitmap, index);
    used_count--;

    // �I�u�W�F�N�g���N���A
    memset(obj, 0, sizeof(Object));
}

//------------------------------------------
// �C���f�b�N�X����
//------------------------------------------
int object_pool_get_index(Object* obj) {
    if (!obj) return -1;

    ptrdiff_t diff = obj - object_pool;
    if (diff < 0 || diff >= OBJECT_POOL_SIZE) {
        return -1;  // �v�[���O�̃I�u�W�F�N�g
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
// ��Ԋm�F
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
// GC�}�[�N����
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
// �f�o�b�O�p�֐�
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
