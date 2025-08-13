// eval.c
// S式の評価器（Evaluator）の実装。

#include "eval.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "object.h"
#include "gc.h"
#include "object_pool.h"
#include "parser.h"
#include "tokenizer.h"
#include "heap.h"

// 環境: ((sym . value) ...) の連鎖リスト（alist）
static Object* g_env = NULL;

// デバッグモード制御
static bool debug_mode = false;

// デバッグ出力マクロ
#define DEBUG_PRINT(...) do { if (debug_mode) printf(__VA_ARGS__); } while(0)

// ユーティリティ
static Object* make_pair(Object* a, Object* b) { return make_cons(a, b); }
static Object* env_lookup(Object* env, const char* name) {
    DEBUG_PRINT("DEBUG: Looking up symbol '%s'\n", name);
    for (Object* it = env; it && it->type == OBJ_CONS; it = it->data.cons.cdr) {
        Object* pair = it->data.cons.car;  // (sym . value)
        if (pair && pair->type == OBJ_CONS) {
            Object* sym = pair->data.cons.car;
            if (sym && sym->type == OBJ_SYMBOL && sym->data.symbol.name) {
                DEBUG_PRINT("DEBUG: Found symbol '%s'\n", sym->data.symbol.name);
                if (strcmp(sym->data.symbol.name, name) == 0) {
                    DEBUG_PRINT("DEBUG: Match found! Returning value\n");
                    return pair->data.cons.cdr;
                }
            }
        }
    }
    DEBUG_PRINT("DEBUG: Symbol '%s' not found in environment\n", name);
    return NULL;
}
static void env_bind(Object** env, const char* name, Object* value) {
    Object* sym  = make_symbol(name);
    Object* pair = make_pair(sym, value);
    *env         = make_pair(pair, *env);
}

// 引数リストの評価
static Object* eval(Object* expr);
static Object* eval_list(Object* list) {
    if (!list || list->type == OBJ_NIL) return obj_nil;
    Object* head = obj_nil;
    Object** cur = &head;
    for (Object* it = list; it && it->type == OBJ_CONS; it = it->data.cons.cdr) {
        Object* ev = eval(it->data.cons.car);
        if (!ev) return obj_nil;
        *cur = make_cons(ev, obj_nil);
        cur  = &((*cur)->data.cons.cdr);
    }
    return head;
}

// ビルトイン: (+ a b ...) / (* a b ...) / (- a b ...) / (/ a b ...)
static Object* builtin_plus(Object* args) {
    DEBUG_PRINT("DEBUG: builtin_plus called\n");
    long sum = 0;
    for (Object* it = args; it && it->type == OBJ_CONS; it = it->data.cons.cdr) {
        Object* a = it->data.cons.car;
        if (!a || a->type != OBJ_NUMBER) return obj_nil;
        sum += a->data.number;
    }
    DEBUG_PRINT("DEBUG: builtin_plus result: %ld\n", sum);
    return make_number((int)sum);
}

static Object* builtin_mul(Object* args) {
    DEBUG_PRINT("DEBUG: builtin_mul called\n");
    long prod = 1;
    for (Object* it = args; it && it->type == OBJ_CONS; it = it->data.cons.cdr) {
        Object* a = it->data.cons.car;
        if (!a || a->type != OBJ_NUMBER) return obj_nil;
        prod *= a->data.number;
    }
    DEBUG_PRINT("DEBUG: builtin_mul result: %ld\n", prod);
    return make_number((int)prod);
}

static Object* builtin_minus(Object* args) {
    if (!args || args->type != OBJ_CONS) return obj_nil;

    Object* first = args->data.cons.car;
    if (!first || first->type != OBJ_NUMBER) return obj_nil;

    // 引数が1つの場合は符号反転
    if (!args->data.cons.cdr || args->data.cons.cdr->type == OBJ_NIL) {
        return make_number(-first->data.number);
    }

    // 複数の引数の場合は最初から順次引く
    long result = first->data.number;
    for (Object* it = args->data.cons.cdr; it && it->type == OBJ_CONS; it = it->data.cons.cdr) {
        Object* a = it->data.cons.car;
        if (!a || a->type != OBJ_NUMBER) return obj_nil;
        result -= a->data.number;
    }
    return make_number((int)result);
}

static Object* builtin_div(Object* args) {
    if (!args || args->type != OBJ_CONS) return obj_nil;

    Object* first = args->data.cons.car;
    if (!first || first->type != OBJ_NUMBER) return obj_nil;

    // 引数が1つの場合は 1/x
    if (!args->data.cons.cdr || args->data.cons.cdr->type == OBJ_NIL) {
        if (first->data.number == 0) return obj_nil; // ゼロ除算エラー
        return make_number(1 / first->data.number);
    }

    // 複数の引数の場合は最初から順次割る
    long result = first->data.number;
    for (Object* it = args->data.cons.cdr; it && it->type == OBJ_CONS; it = it->data.cons.cdr) {
        Object* a = it->data.cons.car;
        if (!a || a->type != OBJ_NUMBER || a->data.number == 0) return obj_nil; // ゼロ除算チェック
        result /= a->data.number;
    }
    return make_number((int)result);
}

static Object* make_builtin(Object* (*fn)(Object*)) {
    Object* f = make_function(fn);
    return f;
}

// 評価本体
static Object* apply(Object* fn, Object* args) {
    if (!fn || fn->type != OBJ_FUNCTION || fn->data.function.native_func == NULL) return obj_nil;
    return fn->data.function.native_func(args);
}

static Object* eval(Object* expr) {
    if (!expr) return obj_nil;
    switch (expr->type) {
        case OBJ_NUMBER:
        case OBJ_STRING:
        case OBJ_LAMBDA:
        case OBJ_FUNCTION:
            return expr;
        case OBJ_SYMBOL: {
            const char* name = expr->data.symbol.name ? expr->data.symbol.name : "";
            DEBUG_PRINT("DEBUG: Evaluating symbol '%s'\n", name);
            Object* v = env_lookup(g_env, name);
            if (debug_mode) {
                printf("DEBUG: Lookup result: ");
                if (v) {
                    object_dump(v);
                } else {
                    printf("NULL");
                }
                printf("\n");
            }
            return v ? v : expr;
        }
        case OBJ_CONS: {
            Object* op        = expr->data.cons.car;
            Object* args_list = expr->data.cons.cdr;

            // 特殊形式 quote
            if (op && op->type == OBJ_SYMBOL && op->data.symbol.name &&
                strcmp(op->data.symbol.name, "quote") == 0) {
                // (quote x) -> x
                if (args_list && args_list->type == OBJ_CONS) {
                    return args_list->data.cons.car;
                }
                return obj_nil;
            }

            // 通常の関数適用（引数は正格評価）
            Object* fn      = eval(op);
            Object* ev_args = eval_list(args_list);
            return apply(fn, ev_args);
        }
        case OBJ_BOOL:
        case OBJ_NIL:
        default:
            return expr;
    }
}

// 公開関数: 文字列入力をパースして評価
Object* eval_string(const char* src) {
    gc_add_root(&g_env);
    Object* ast = parse(src);
    if (!ast) {
        DEBUG_PRINT("DEBUG: parse() returned NULL\n");
        gc_remove_root(&g_env);
        return obj_nil;
    }
    if (debug_mode) {
        printf("DEBUG: parsed AST: ");
        object_dump(ast);
        printf("\n");
    }

    gc_add_root(&ast);
    Object* result = eval(ast);
    if (debug_mode) {
        printf("DEBUG: eval result: ");
        if (result) {
            object_dump(result);
        } else {
            printf("NULL");
        }
        printf("\n");
    }

    gc_add_root(&result);
    // GCを1回走らせる（リーク検出兼ねる）
    gc_collect();
    gc_remove_root(&result);
    gc_remove_root(&ast);
    gc_remove_root(&g_env);
    return result ? result : obj_nil;
}

// 初期化/終了
void evaluator_init(void) {
    // オブジェクトシステム全体を初期化
    object_system_init();

    g_env = obj_nil;
    DEBUG_PRINT("DEBUG: g_env initialized to obj_nil\n");
    gc_add_root(&g_env);

    // ビルトイン登録
    DEBUG_PRINT("DEBUG: Registering builtin '+'\n");
    Object* plus_func = make_builtin(builtin_plus);
    if (debug_mode) {
        printf("DEBUG: Created plus function: ");
        object_dump(plus_func);
        printf("\n");
    }

    env_bind(&g_env, "+", plus_func);
    if (debug_mode) {
        printf("DEBUG: Environment after binding '+':\n");
        object_dump(g_env);
        printf("\n");
    }

    env_bind(&g_env, "*", make_builtin(builtin_mul));
    env_bind(&g_env, "-", make_builtin(builtin_minus));
    env_bind(&g_env, "/", make_builtin(builtin_div));

    gc_remove_root(&g_env);
}

// デバッグモード設定
void evaluator_set_debug(bool enable) {
    debug_mode = enable;
}

void evaluator_shutdown(void) {
    // 必要ならリソース解放（プール + GCが回収する）
}

// メモリ統計表示
void evaluator_show_memory_stats(void) {
    printf("\n=== Memory Statistics ===\n");

    // オブジェクトプールの統計
    size_t used = object_pool_used_count();
    size_t free = object_pool_free_count();
    size_t total = OBJECT_POOL_SIZE;

    printf("Object Pool:\n");
    printf("  Total objects: %zu\n", total);
    printf("  Used objects:  %zu (%.1f%%)\n", used, (double)used / total * 100.0);
    printf("  Free objects:  %zu (%.1f%%)\n", free, (double)free / total * 100.0);
    printf("  Memory usage:  %zu bytes (%zu KB)\n", used * sizeof(Object), (used * sizeof(Object)) / 1024);
    printf("  Object size:   %zu bytes each\n", sizeof(Object));

    // ヒープの統計
    printf("\nVariable Data Heap:\n");
    printf("  Total size:    %zu bytes (%zu KB)\n", heap_total_size(), heap_total_size() / 1024);
    printf("  Used size:     %zu bytes (%.1f%%)\n", heap_used_size(), (double)heap_used_size() / heap_total_size() * 100.0);
    printf("  Free size:     %zu bytes (%.1f%%)\n", heap_free_size(), (double)heap_free_size() / heap_total_size() * 100.0);
    printf("  Allocated chunks: %zu\n", heap_allocated_chunks());
    printf("  Free chunks:      %zu\n", heap_free_chunks());

    // GCの統計
    printf("\nGarbage Collector:\n");
    printf("  Total collections: %zu\n", gc_total_collections());
    printf("  Last collected:    %zu objects\n", gc_last_collected_count());
    printf("  Total collected:   %zu objects\n", gc_total_collected_count());

    // 効率性の指標
    if (gc_total_collections() > 0) {
        double avg_collected = (double)gc_total_collected_count() / gc_total_collections();
        printf("  Avg per collection: %.1f objects\n", avg_collected);
    }

    printf("========================\n\n");
}