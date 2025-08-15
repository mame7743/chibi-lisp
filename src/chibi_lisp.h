// chibi_lisp.h
// Core configuration and constants for chibi-lisp

#ifndef CHIBI_LISP_H
#define CHIBI_LISP_H

//------------------------------------------
// Memory Configuration (efficient but not minimal)
//------------------------------------------

#define KB (1024)
#define MB (KB * 1024)

// Object pool - 効率的だが過度にメモリを使わない
#define OBJECT_POOL_SIZE 1024

// Heap for variable-length data
#define HEAP_SIZE (1*MB)       // 1MB
#define CHUNK_SIZE 32         // 32バイトチャンク
#define CHUNK_COUNT (HEAP_SIZE / CHUNK_SIZE)

// Bitmap size calculation
#define BITMAP_SIZE ((OBJECT_POOL_SIZE + 7) / 8)

// GC and evaluation limits (conservative for embedded compatibility)
#define MAX_ROOTS 32              // GCルートオブジェクト数
#define MAX_RECURSION_DEPTH 100   // 再帰の最大深度
#define MAX_EVAL_STACK 256        // 評価スタック
#define MAX_GC_MARK_STACK 256     // GCマークスタック

// Buffer sizes
#define MAX_INPUT_LINE 512        // 入力行の最大長
#define MAX_SYMBOL_LENGTH 64      // シンボル名の最大長
#define MAX_STRING_LENGTH 256     // 文字列の最大長

// Feature flags (essential features only)
#define FEATURE_DEBUG_MODE 1      // デバッグ機能
#define FEATURE_MEMORY_STATS 1    // メモリ統計
#define FEATURE_GC_STATS 1        // GC統計

#endif // CHIBI_LISP_H
