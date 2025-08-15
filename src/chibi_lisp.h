// chibi_lisp.h
// Core configuration and constants for chibi-lisp

#ifndef CHIBI_LISP_H
#define CHIBI_LISP_H

//------------------------------------------
// Memory Configuration (efficient but not minimal)
//------------------------------------------

#define KB (1024)
#define MB (KB * 1024)

// Object pool - �����I�����ߓx�Ƀ��������g��Ȃ�
#define OBJECT_POOL_SIZE 1024

// Heap for variable-length data
#define HEAP_SIZE (1*MB)       // 1MB
#define CHUNK_SIZE 32         // 32�o�C�g�`�����N
#define CHUNK_COUNT (HEAP_SIZE / CHUNK_SIZE)

// Bitmap size calculation
#define BITMAP_SIZE ((OBJECT_POOL_SIZE + 7) / 8)

// GC and evaluation limits (conservative for embedded compatibility)
#define MAX_ROOTS 32              // GC���[�g�I�u�W�F�N�g��
#define MAX_RECURSION_DEPTH 100   // �ċA�̍ő�[�x
#define MAX_EVAL_STACK 256        // �]���X�^�b�N
#define MAX_GC_MARK_STACK 256     // GC�}�[�N�X�^�b�N

// Buffer sizes
#define MAX_INPUT_LINE 512        // ���͍s�̍ő咷
#define MAX_SYMBOL_LENGTH 64      // �V���{�����̍ő咷
#define MAX_STRING_LENGTH 256     // ������̍ő咷

// Feature flags (essential features only)
#define FEATURE_DEBUG_MODE 1      // �f�o�b�O�@�\
#define FEATURE_MEMORY_STATS 1    // ���������v
#define FEATURE_GC_STATS 1        // GC���v

#endif // CHIBI_LISP_H
