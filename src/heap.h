#ifndef __HEAP_H__
#define __HEAP_H__

#include <stddef.h>

// ヒープ関数の宣言
void heap_init(void);
void* heap_alloc(size_t size);
void heap_free(void *ptr);  // sizeパラメータを削除

// デバッグ関数
void heap_dump(void);

#endif // __HEAP_H__