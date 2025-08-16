#ifndef OBJECT_POOL_H
#define OBJECT_POOL_H

#include "chibi_lisp.h"
#include "object.h"
#include <stdint.h>
#include <stddef.h>

#define BITMAP_SIZE ((OBJECT_POOL_SIZE + 7) / 8)  // ビットマップサイズ

// 外部からアクセス可能なプールデータ（テスト用）
extern Object object_pool[OBJECT_POOL_SIZE];
extern uint8_t allocation_bitmap[BITMAP_SIZE];
extern uint8_t marked_bitmap[BITMAP_SIZE];

//------------------------------------------
// オブジェクトプール管理
//------------------------------------------
void object_pool_init(void);
Object* object_pool_alloc(void);
void object_pool_free(Object* obj);

// インデックス操作
int object_pool_get_index(Object* obj);
Object* object_pool_get_object(int index);

// 状態確認
bool object_pool_is_allocated(int index);
size_t object_pool_used_count(void);
size_t object_pool_free_count(void);
bool object_pool_is_valid(Object* obj);

// GCマーク操作
bool object_pool_is_marked(int index);
void object_pool_set_mark(int index);
void object_pool_clear_mark(int index);
void object_pool_clear_all_marks(void);

//------------------------------------------
// デバッグ用
//------------------------------------------
void object_pool_dump(void);
void object_pool_dump_bitmap(void);

#endif // OBJECT_POOL_H