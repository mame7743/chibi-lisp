// repl.c
// 対話型シェル（REPL: Read Eval Print Loop）の実装。

#include <stdio.h>
#include <string.h>

#include "object.h"
#include "eval.h"  // 評価関数のインターフェース
#include "gc.h"    // GC の明示呼び出し

static void print_obj(Object* obj) {
    if (!obj) {
        printf("NULL");
        return;
    }

    switch (obj->type) {
        case OBJ_NIL:
            printf("nil");
            break;
        case OBJ_BOOL:
            printf(obj == obj_true ? "t" : "nil");
            break;
        case OBJ_NUMBER:
            printf("%d", obj->data.number);
            break;
        case OBJ_SYMBOL:
            printf("%s", obj->data.symbol.name ? obj->data.symbol.name : "");
            break;
        case OBJ_STRING:
            printf("\"%s\"", obj->data.string.text ? obj->data.string.text : "");
            break;
        case OBJ_CONS: {
            printf("(");
            Object* it = obj;
            while (it && it->type == OBJ_CONS) {
                print_obj(it->data.cons.car);
                it = it->data.cons.cdr;
                if (it && it->type == OBJ_CONS) printf(" ");
            }
            if (it && it->type != OBJ_NIL) {
                printf(" . ");
                print_obj(it);
            }
            printf(")");
            break;
        }
        case OBJ_FUNCTION:
            printf("#<function>");
            break;
        case OBJ_LAMBDA:
            printf("#<lambda>");
            break;
        case OBJ_OPERATOR:
            printf("%s", obj_operator_name(obj));
            break;
        case OBJ_VOID:
            // voidは表示しない（この関数は呼ばれないはず）
            break;
        default:
            printf("#<unknown-type-%d>", obj->type);
            break;
    }
}

int main(int argc, char* argv[]) {
    bool debug_enabled = false;

    // コマンドライン引数を処理
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--debug") == 0 || strcmp(argv[i], "-d") == 0) {
            debug_enabled = true;
        } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            printf("Usage: %s [options]\n", argv[0]);
            printf("Options:\n");
            printf("  --debug, -d    Enable debug output\n");
            printf("  --help, -h     Show this help message\n");
            printf("\nREPL Commands:\n");
            printf("  :quit          Exit the REPL\n");
            printf("  :mem           Show memory statistics\n");
            printf("  :gc            Run garbage collection manually\n");
            return 0;
        }
    }

    evaluator_init();
    evaluator_set_debug(debug_enabled);

    if (debug_enabled) {
        printf("Debug mode enabled.\n");
    }

    char line[1024];
    printf("chibi-lisp REPL. Type :quit to exit, :mem for memory stats.\n");
    while (1) {
        printf("> ");
        if (!fgets(line, sizeof(line), stdin)) break;
        if (strncmp(line, ":quit", 5) == 0) break;
        if (strncmp(line, ":mem", 4) == 0) {
            evaluator_show_memory_stats();
            continue;
        }
        if (strncmp(line, ":gc", 3) == 0) {
            gc_collect();
            printf("[GC done] total=%zu last=%zu collected=%zu\n",
                   gc_total_collections(),
                   gc_last_collected_count(),
                   gc_total_collected_count());
            continue;
        }

        Object* res = eval_string(line);
        // OBJ_VOID型（print関数等の戻り値）の場合のみ表示を抑制
        // 明示的なnilは表示する
        if (res && res->type != OBJ_VOID) {
            print_obj(res);
            printf("\n");
        }
    }

    evaluator_shutdown();
    return 0;
}