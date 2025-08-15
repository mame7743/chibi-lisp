// eval.c
// S式の評価器（Evaluator）の実装。

#include "eval.h"
#include "chibi_lisp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <limits.h>

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

// スコープ管理のヘルパー関数
static Object* env_push_scope(Object* env, const char* name, Object* value) {
    // 新しいスコープを作成して変数をバインド
    Object* sym = make_symbol(name);
    Object* pair = make_pair(sym, value);
    return make_pair(pair, env);  // 新しい環境を返す
}

static Object* env_pop_scope(Object* env) {
    // 一つ前のスコープに戻る
    if (env && env->type == OBJ_CONS) {
        return env->data.cons.cdr;  // 親スコープを返す
    }
    return env;
}
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
static Object* eval_with_env(Object* expr, Object* env);

// ビルトイン関数の前方宣言
static Object* builtin_plus(Object* args);
static Object* builtin_minus(Object* args);
static Object* builtin_mul(Object* args);
static Object* builtin_div(Object* args);
static Object* builtin_eq(Object* args);
static Object* builtin_lt(Object* args);
static Object* builtin_gt(Object* args);
static Object* builtin_lte(Object* args);
static Object* builtin_gte(Object* args);
static Object* builtin_print(Object* args);
static Object* builtin_println(Object* args);
static Object* builtin_str(Object* args);
static Object* builtin_length(Object* args);
static Object* builtin_boolp(Object* args);
// タイマー関数の前方宣言
static Object* builtin_now(Object* args);
static Object* builtin_sleep(Object* args);
static Object* builtin_time_diff(Object* args);
// ループ制御関数の前方宣言
static Object* builtin_dotimes(Object* args);  // Common Lisp標準
// 特別処理関数の前方宣言
static Object* eval_dotimes_special(Object* args);
// dotimesヘルパー関数の前方宣言
static bool parse_dotimes_args(Object* args, const char** var_name, int* count, Object** expressions);
static Object* execute_dotimes_loop(const char* var_name, int count, Object* expressions);
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

static Object* eval_list_with_env(Object* list, Object* env) {
    if (!list || list->type == OBJ_NIL) return obj_nil;
    Object* head = obj_nil;
    Object** cur = &head;
    for (Object* it = list; it && it->type == OBJ_CONS; it = it->data.cons.cdr) {
        Object* ev = eval_with_env(it->data.cons.car, env);
        if (!ev) return obj_nil;
        *cur = make_cons(ev, obj_nil);
        cur  = &((*cur)->data.cons.cdr);
    }
    return head;
}

// メイン評価関数の実装
static Object* eval_with_env(Object* expr, Object* env) {
    if (!expr) return obj_nil;

    switch (expr->type) {
        case OBJ_NIL:
        case OBJ_BOOL:
        case OBJ_NUMBER:
        case OBJ_STRING:
        case OBJ_FUNCTION:
        case OBJ_LAMBDA:
        case OBJ_OPERATOR:
        case OBJ_BUILTIN:
        case OBJ_VOID:
            return expr; // 自己評価

        case OBJ_SYMBOL: {
            Object* value = env_lookup(env, expr->data.symbol.name);
            return value ? value : obj_nil;
        }

        case OBJ_CONS: {
            // 特別な構文の処理は eval() で行うため、ここでは通常の関数呼び出しのみ
            Object* func = eval_with_env(expr->data.cons.car, env);
            if (!func) return obj_nil;

            // 引数リストを環境付きで評価
            Object* args = eval_list_with_env(expr->data.cons.cdr, env);

            if (func->type == OBJ_FUNCTION && func->data.function.native_func) {
                return func->data.function.native_func(args);
            } else if (func->type == OBJ_OPERATOR) {
                // 演算子処理
                switch (func->data.operator_type) {
                    case OP_PLUS: return builtin_plus(args);
                    case OP_MINUS: return builtin_minus(args);
                    case OP_ASTERISK: return builtin_mul(args);
                    case OP_SLASH: return builtin_div(args);
                    case OP_EQ: return builtin_eq(args);
                    case OP_LT: return builtin_lt(args);
                    case OP_GT: return builtin_gt(args);
                    case OP_LTE: return builtin_lte(args);
                    case OP_GTE: return builtin_gte(args);
                    default: return obj_nil;
                }
            } else if (func->type == OBJ_BUILTIN) {
                // 組み込み関数処理
                switch (func->data.builtin_type) {
                    case BUILTIN_PRINT: return builtin_print(args);
                    case BUILTIN_PRINTLN: return builtin_println(args);
                    case BUILTIN_STR: return builtin_str(args);
                    case BUILTIN_LENGTH: return builtin_length(args);
                    case BUILTIN_BOOLP: return builtin_boolp(args);
                    case BUILTIN_NOW: return builtin_now(args);
                    case BUILTIN_SLEEP: return builtin_sleep(args);
                    case BUILTIN_TIME_DIFF: return builtin_time_diff(args);
                    case BUILTIN_DOTIMES: return builtin_dotimes(args);
                    default: return obj_nil;
                }
            }
            return obj_nil;
        }

        default:
            return obj_nil;
    }
}

static Object* eval(Object* expr) {
    if (!expr) return obj_nil;

    // 特別な構文の処理
    if (expr->type == OBJ_CONS) {
        // dotimesの特別処理
        if (expr->data.cons.car && expr->data.cons.car->type == OBJ_SYMBOL &&
            expr->data.cons.car->data.symbol.name &&
            strcmp(expr->data.cons.car->data.symbol.name, "dotimes") == 0) {
            return eval_dotimes_special(expr->data.cons.cdr);
        }
    }

    // 通常の評価
    return eval_with_env(expr, g_env);
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

// 比較演算子
static Object* builtin_eq(Object* args) {
    if (!args || args->type != OBJ_CONS || !args->data.cons.cdr || args->data.cons.cdr->type != OBJ_CONS) {
        return obj_nil;
    }
    Object* a = args->data.cons.car;
    Object* b = args->data.cons.cdr->data.cons.car;
    if (!a || !b) return obj_nil;

    // ポインタが同じ場合は等しい（nil同士、true同士など）
    if (a == b) return obj_true;

    if (a->type == OBJ_NUMBER && b->type == OBJ_NUMBER) {
        return (a->data.number == b->data.number) ? obj_true : obj_nil;
    }
    return obj_nil;
}

static Object* builtin_lt(Object* args) {
    if (!args || args->type != OBJ_CONS || !args->data.cons.cdr || args->data.cons.cdr->type != OBJ_CONS) {
        return obj_nil;
    }
    Object* a = args->data.cons.car;
    Object* b = args->data.cons.cdr->data.cons.car;
    if (!a || !b || a->type != OBJ_NUMBER || b->type != OBJ_NUMBER) return obj_nil;

    return (a->data.number < b->data.number) ? obj_true : obj_nil;
}

static Object* builtin_gt(Object* args) {
    if (!args || args->type != OBJ_CONS || !args->data.cons.cdr || args->data.cons.cdr->type != OBJ_CONS) {
        return obj_nil;
    }
    Object* a = args->data.cons.car;
    Object* b = args->data.cons.cdr->data.cons.car;
    if (!a || !b || a->type != OBJ_NUMBER || b->type != OBJ_NUMBER) return obj_nil;

    return (a->data.number > b->data.number) ? obj_true : obj_nil;
}

static Object* builtin_lte(Object* args) {
    if (!args || args->type != OBJ_CONS || !args->data.cons.cdr || args->data.cons.cdr->type != OBJ_CONS) {
        return obj_nil;
    }
    Object* a = args->data.cons.car;
    Object* b = args->data.cons.cdr->data.cons.car;
    if (!a || !b || a->type != OBJ_NUMBER || b->type != OBJ_NUMBER) return obj_nil;

    return (a->data.number <= b->data.number) ? obj_true : obj_nil;
}

static Object* builtin_gte(Object* args) {
    if (!args || args->type != OBJ_CONS || !args->data.cons.cdr || args->data.cons.cdr->type != OBJ_CONS) {
        return obj_nil;
    }
    Object* a = args->data.cons.car;
    Object* b = args->data.cons.cdr->data.cons.car;
    if (!a || !b || a->type != OBJ_NUMBER || b->type != OBJ_NUMBER) return obj_nil;

    return (a->data.number >= b->data.number) ? obj_true : obj_nil;
}

// ---- 追加: 出力/ユーティリティ系ビルトイン ----
static void print_object_repr(Object* obj, bool newline) {
    if (!obj) { printf("nil"); if (newline) printf("\n"); return; }
    switch (obj->type) {
        case OBJ_NIL:    printf("nil"); break;
        case OBJ_BOOL:   printf(obj == obj_true ? "t" : "nil"); break;
        case OBJ_NUMBER: printf("%d", obj->data.number); break;
        case OBJ_STRING: printf("%s", obj->data.string.text ? obj->data.string.text : ""); break;
        case OBJ_SYMBOL: printf("%s", obj->data.symbol.name ? obj->data.symbol.name : ""); break;
        case OBJ_CONS: {
            printf("(");
            Object* it = obj;
            bool first = true;
            while (it && it->type == OBJ_CONS) {
                if (!first) printf(" ");
                print_object_repr(it->data.cons.car, false);
                it = it->data.cons.cdr;
                first = false;
            }
            if (it && it != obj_nil) { // ドットリスト
                printf(" . ");
                print_object_repr(it, false);
            }
            printf(")");
            break;
        }
        case OBJ_FUNCTION: printf("<function>"); break;
        case OBJ_LAMBDA:   printf("<lambda>"); break;
        default:           printf("<unknown>"); break;
    }
    if (newline) printf("\n");
}

static Object* builtin_print(Object* args) {
    for (Object* it = args; it && it->type == OBJ_CONS; it = it->data.cons.cdr) {
        print_object_repr(it->data.cons.car, false);
        if (it->data.cons.cdr && it->data.cons.cdr->type == OBJ_CONS) printf(" ");
    }
    printf("\n");
    return obj_void;
}

static Object* builtin_println(Object* args) { // alias; ensure newline after each arg
    for (Object* it = args; it && it->type == OBJ_CONS; it = it->data.cons.cdr) {
        print_object_repr(it->data.cons.car, true);
    }
    return obj_void;
}

static Object* builtin_str(Object* args) { // 連結して文字列化
    // まず合計長計算
    size_t total = 0;
    for (Object* it = args; it && it->type == OBJ_CONS; it = it->data.cons.cdr) {
        Object* a = it->data.cons.car;
        if (!a) continue;
        switch (a->type) {
            case OBJ_NUMBER: {
                char buf[32];
                snprintf(buf, sizeof(buf), "%d", a->data.number);
                total += strlen(buf);
                break;
            }
            case OBJ_STRING: if (a->data.string.text) total += a->data.string.length; break;
            case OBJ_SYMBOL: if (a->data.symbol.name) total += a->data.symbol.length; break;
            case OBJ_NIL: total += 3; break; // "nil"
            case OBJ_BOOL: total += (a == obj_true) ? 1 : 3; break; // t/nil
            default: total += 9; break; // <unknown>
        }
    }
    if (total == 0) return make_string("");
    char *buf = (char*)heap_alloc(total + 1);
    if (!buf) return obj_nil;
    size_t pos = 0;
    for (Object* it = args; it && it->type == OBJ_CONS; it = it->data.cons.cdr) {
        Object* a = it->data.cons.car;
        if (!a) continue;
        if (a->type == OBJ_NUMBER) {
            char nbuf[32];
            int n = snprintf(nbuf, sizeof(nbuf), "%d", a->data.number);
            memcpy(buf + pos, nbuf, n); pos += n;
        } else if (a->type == OBJ_STRING && a->data.string.text) {
            memcpy(buf + pos, a->data.string.text, a->data.string.length); pos += a->data.string.length;
        } else if (a->type == OBJ_SYMBOL && a->data.symbol.name) {
            memcpy(buf + pos, a->data.symbol.name, a->data.symbol.length); pos += a->data.symbol.length;
        } else if (a == obj_nil) {
            memcpy(buf + pos, "nil", 3); pos += 3;
        } else if (a->type == OBJ_BOOL) {
            if (a == obj_true) { memcpy(buf + pos, "t", 1); pos += 1; }
            else { memcpy(buf + pos, "nil", 3); pos += 3; }
        } else {
            memcpy(buf + pos, "<unknown>", 9); pos += 9;
        }
    }
    buf[pos] = '\0';
    return make_string(buf);
}

static Object* builtin_length(Object* args) {
    if (!args || args->type != OBJ_CONS) return obj_nil;
    Object* target = args->data.cons.car;
    int count = 0;
    while (target && target->type == OBJ_CONS) {
        count++;
        target = target->data.cons.cdr;
    }
    return make_number(count);
}

static Object* builtin_boolp(Object* args) {
    if (!args || args->type != OBJ_CONS) return obj_nil;
    Object* a = args->data.cons.car;
    if (!a) return obj_nil;
    return (a->type == OBJ_BOOL) ? obj_true : obj_nil;
}

// ---- タイマー関数 ----
static Object* builtin_now(Object* args) {
    // 現在時刻をミリ秒で取得
    struct timeval tv;
    gettimeofday(&tv, NULL);
    long long milliseconds = (long long)tv.tv_sec * 1000 + tv.tv_usec / 1000;
    return make_number((int)(milliseconds % INT_MAX)); // オーバーフロー対策
}

static Object* builtin_sleep(Object* args) {
    if (!args || args->type != OBJ_CONS) return obj_nil;
    Object* duration = args->data.cons.car;
    if (!duration || duration->type != OBJ_NUMBER) return obj_nil;

    // 秒数で指定（小数点は切り捨て）
    int seconds = duration->data.number;
    if (seconds > 0) {
        sleep(seconds);
    }
    return obj_nil;
}

static Object* builtin_time_diff(Object* args) {
    // 2つの時刻の差を計算（ミリ秒）
    if (!args || args->type != OBJ_CONS) return obj_nil;
    Object* time1 = args->data.cons.car;
    if (!time1 || time1->type != OBJ_NUMBER) return obj_nil;

    Object* rest = args->data.cons.cdr;
    if (!rest || rest->type != OBJ_CONS) return obj_nil;
    Object* time2 = rest->data.cons.car;
    if (!time2 || time2->type != OBJ_NUMBER) return obj_nil;

    int diff = time2->data.number - time1->data.number;
    return make_number(diff);
}

// ---- ループ制御関数 ----

// dotimes引数パースのヘルパー関数
static bool parse_dotimes_args(Object* args, const char** var_name, int* count, Object** expressions) {
    // 基本構造チェック: (dotimes (var count) expr1 expr2 ...)
    if (!args || args->type != OBJ_CONS) {
        DEBUG_PRINT("DEBUG: dotimes: invalid arguments structure\n");
        return false;
    }

    // 最初の引数は (var count) の形式
    Object* var_count = args->data.cons.car;
    if (!var_count || var_count->type != OBJ_CONS) {
        DEBUG_PRINT("DEBUG: dotimes: first argument must be (var count)\n");
        return false;
    }

    // 変数名を取得
    Object* var_name_obj = var_count->data.cons.car;
    if (!var_name_obj || var_name_obj->type != OBJ_SYMBOL) {
        DEBUG_PRINT("DEBUG: dotimes: variable name must be symbol\n");
        return false;
    }
    *var_name = var_name_obj->data.symbol.name;

    // カウントを取得・評価
    Object* count_list = var_count->data.cons.cdr;
    if (!count_list || count_list->type != OBJ_CONS) {
        DEBUG_PRINT("DEBUG: dotimes: count argument missing\n");
        return false;
    }

    Object* count_obj = eval(count_list->data.cons.car);
    if (!count_obj || count_obj->type != OBJ_NUMBER) {
        DEBUG_PRINT("DEBUG: dotimes: count must evaluate to number\n");
        return false;
    }

    *count = count_obj->data.number;
    if (*count < 0) {
        DEBUG_PRINT("DEBUG: dotimes: negative count not allowed: %d\n", *count);
        return false;
    }

    // 式リストを取得
    *expressions = args->data.cons.cdr;
    return true;
}

// dotimesループ実行のヘルパー関数
static Object* execute_dotimes_loop(const char* var_name, int count, Object* expressions) {
    Object* last_result = obj_nil;

    DEBUG_PRINT("DEBUG: dotimes starting loop, count=%d, var=%s\n", count, var_name);

    // 0からcount-1まで実行（Common Lisp標準）
    for (int i = 0; i < count; i++) {
        DEBUG_PRINT("DEBUG: dotimes iteration %d\n", i);

        // 新しいスコープを作成して変数をバインド
        Object* scoped_env = env_push_scope(g_env, var_name, make_number(i));

        // 式を実行
        for (Object* it = expressions; it && it->type == OBJ_CONS; it = it->data.cons.cdr) {
            DEBUG_PRINT("DEBUG: evaluating expression in iteration %d\n", i);
            last_result = eval_with_env(it->data.cons.car, scoped_env);
        }

        // スコープは自動的に破棄される（GCが回収）
    }

    DEBUG_PRINT("DEBUG: dotimes completed\n");
    return last_result;
}

static Object* eval_dotimes_special(Object* args) {
    // (dotimes (var count) expr1 expr2 ...)
    // Common Lisp標準形式：0からcount-1まで変数varをインクリメントしながら実行

    const char* var_name;
    int count;
    Object* expressions;

    // 引数をパース
    if (!parse_dotimes_args(args, &var_name, &count, &expressions)) {
        return obj_nil;
    }

    // countが0の場合は何もしない
    if (count == 0) {
        DEBUG_PRINT("DEBUG: dotimes: count is 0, skipping loop\n");
        return obj_nil;
    }

    // ループを実行
    return execute_dotimes_loop(var_name, count, expressions);
}

static Object* builtin_dotimes(Object* args) {
    // 通常の関数呼び出しの場合（引数は既に評価済み）
    return eval_dotimes_special(args);
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

    g_env = NULL; // 初期化: obj_nilはマクロで関数呼び出しになったため、直接代入は避ける
    g_env = obj_nil;
    DEBUG_PRINT("DEBUG: g_env initialized to obj_nil\n");
    gc_add_root(&g_env);    // ビルトイン登録
    DEBUG_PRINT("DEBUG: Registering builtin functions\n");

    env_bind(&g_env, "+", make_function(builtin_plus));
    env_bind(&g_env, "*", make_function(builtin_mul));
    env_bind(&g_env, "-", make_function(builtin_minus));
    env_bind(&g_env, "/", make_function(builtin_div));

    // 比較演算子の登録
    env_bind(&g_env, "=", make_function(builtin_eq));
    env_bind(&g_env, "<", make_function(builtin_lt));
    env_bind(&g_env, ">", make_function(builtin_gt));
    env_bind(&g_env, "<=", make_function(builtin_lte));
    env_bind(&g_env, ">=", make_function(builtin_gte));
    env_bind(&g_env, "print", make_function(builtin_print));
    env_bind(&g_env, "println", make_function(builtin_println));
    env_bind(&g_env, "str", make_function(builtin_str));
    env_bind(&g_env, "length", make_function(builtin_length));
    env_bind(&g_env, "bool?", make_function(builtin_boolp));

    // タイマー関数の登録
    env_bind(&g_env, "now", make_function(builtin_now));
    env_bind(&g_env, "sleep", make_function(builtin_sleep));
    env_bind(&g_env, "time-diff", make_function(builtin_time_diff));

    // ループ制御関数の登録
    env_bind(&g_env, "dotimes", make_function(builtin_dotimes));

    gc_remove_root(&g_env);
}

// デバッグモード設定
void evaluator_set_debug(bool enable) {
    debug_mode = enable;
}

void evaluator_shutdown(void) {
    // 必要ならリソース解放（プール + GCが回収する）
}