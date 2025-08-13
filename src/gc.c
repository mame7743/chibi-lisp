#include "gc.h"
#include <stdio.h>
#include <string.h>

//------------------------------------------
// GC�f�[�^
//------------------------------------------
static Object** gc_roots[MAX_ROOTS];
static size_t gc_root_count = 0;
static size_t gc_collections = 0;
static size_t gc_last_collected = 0;
static size_t gc_total_collected = 0;

//------------------------------------------
// �O���Q�Ɓi�Œ�I�u�W�F�N�g�j
//------------------------------------------
extern Object* obj_nil;
extern Object* obj_true;
extern Object* obj_false;

//------------------------------------------
// GC������
//------------------------------------------
void gc_init(void) {
    gc_root_count = 0;
    gc_collections = 0;
    gc_last_collected = 0;
    gc_total_collected = 0;
    memset(gc_roots, 0, sizeof(gc_roots));
}

//------------------------------------------
// ���[�g�Z�b�g�Ǘ�
//------------------------------------------
void gc_add_root(Object** root) {
    if (gc_root_count < MAX_ROOTS && root != NULL) {
        gc_roots[gc_root_count++] = root;
    }
}

void gc_remove_root(Object** root) {
    for (size_t i = 0; i < gc_root_count; i++) {
        if (gc_roots[i] == root) {
            // �Ō�̗v�f�����݂̈ʒu�Ɉړ�
            gc_roots[i] = gc_roots[--gc_root_count];
            gc_roots[gc_root_count] = NULL;
            break;
        }
    }
}

//------------------------------------------
// �}�[�N����
//------------------------------------------
static void gc_mark_object(Object* obj) {
    if (!obj) return;

    // �v�[�����̃I�u�W�F�N�g���`�F�b�N
    if (!object_pool_is_valid(obj)) return;

    int index = object_pool_get_index(obj);
    if (index < 0 || object_pool_is_marked(index)) return;

    // �r�b�g�}�b�v�Ń}�[�N
    object_pool_set_mark(index);

    // �q�I�u�W�F�N�g���ċA�I�Ƀ}�[�N
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
            // �v���~�e�B�u�^�͎q�I�u�W�F�N�g�������Ȃ�
            break;
    }
}

//------------------------------------------
// �K�x�[�W�R���N�V�������s
//------------------------------------------
void gc_collect(void) {
    gc_collections++;
    gc_last_collected = 0;

    // �}�[�N�t�F�[�Y: �S�I�u�W�F�N�g�̃}�[�N���N���A
    object_pool_clear_all_marks();

    // ���[�g�I�u�W�F�N�g����}�[�N
    for (size_t i = 0; i < gc_root_count; i++) {
        if (gc_roots[i] && *gc_roots[i]) {
            gc_mark_object(*gc_roots[i]);
        }
    }

    // �Œ�I�u�W�F�N�g�͏�ɐ����iobject_pool�O�Ȃ̂ŉ������Ȃ��j
    // obj_nil, obj_true, obj_false��static�̈�ɂ��邽�߁A
    // �r�b�g�}�b�v�ł̃}�[�N�͕s�v

    // �X�C�[�v�t�F�[�Y: �}�[�N����Ă��Ȃ��I�u�W�F�N�g�����
    for (int i = 0; i < OBJECT_POOL_SIZE; i++) {
        if (object_pool_is_allocated(i)) {
            Object* obj = object_pool_get_object(i);

            // �Œ�I�u�W�F�N�g��object_pool�O�Ȃ̂ŁA�����ɂ͌���Ȃ�
            if (!object_pool_is_marked(i)) {
                object_pool_free(obj);
                gc_last_collected++;
            }
        }
    }

    gc_total_collected += gc_last_collected;
}

//------------------------------------------
// GC���v
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
// �f�o�b�O�p�֐�
//------------------------------------------
void gc_dump_roots(void) {
    printf("GC Root Objects:\n");
    for (size_t i = 0; i < gc_root_count; i++) {
        printf("  Root %zu: %p -> ", i, (void*)gc_roots[i]);
        if (gc_roots[i] && *gc_roots[i]) {
            extern void object_dump(Object* obj); // Object.c����Q��
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