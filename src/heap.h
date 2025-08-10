#ifndef __HEAP_H__
#define __HEAP_H__

#include <stddef.h>

// �q�[�v�֐��̐錾
void heap_init(void);
void* heap_alloc(size_t size);
void heap_free(void *ptr);  // size�p�����[�^���폜

// �f�o�b�O�֐�
void heap_dump(void);

#endif // __HEAP_H__