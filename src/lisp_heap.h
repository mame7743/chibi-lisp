// lisp_heap.h
#ifndef LISP_HEAP_H
#define LISP_HEAP_H

#include <stddef.h>
#include "value.h"

void lisp_heap_init(void);
Value *lisp_heap_alloc(void);
void lisp_heap_free(Value *v);

#endif
