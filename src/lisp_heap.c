// lisp_heap.c
#include "lisp_heap.h"
#include <string.h>
#include <stdlib.h>

#define LISP_HEAP_SIZE 1024

static Value heap_pool[LISP_HEAP_SIZE];
static Value *free_list = NULL;

void lisp_heap_init(void) {
    free_list = &heap_pool[0];
    for (size_t i = 0; i < LISP_HEAP_SIZE - 1; i++) {
        heap_pool[i].data.cons.cdr = (Value *)&heap_pool[i + 1];
    }
    heap_pool[LISP_HEAP_SIZE - 1].data.cons.cdr = NULL;
}

Value *lisp_heap_alloc(void) {
    if (!free_list) return NULL;  // ƒƒ‚ƒŠ•s‘«
    Value *v  = free_list;
    free_list = v->data.cons.cdr;
    return v;
}

void lisp_heap_free(Value *v) {
    v->data.cons.cdr = free_list;
    free_list = v;
}
