#ifndef GC_H
#define GC_H

#include "chibi_lisp.h"
#include "object.h"
#include "object_pool.h"

//------------------------------------------
// ガベージコレクション
//------------------------------------------
void gc_init(void);
void gc_collect(void);

// ルートセットの管理
void gc_add_root(Object** root);
void gc_remove_root(Object** root);

// GC統計
size_t gc_total_collections(void);
size_t gc_last_collected_count(void);
size_t gc_total_collected_count(void);

//------------------------------------------
// デバッグ用
//------------------------------------------
void gc_dump_roots(void);
void gc_dump_stats(void);

#endif // GC_H