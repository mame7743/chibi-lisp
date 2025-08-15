// main.c
// chibi-lisp ????????????

#include <stdio.h>
#include <string.h>

#include "chibi_lisp.h"
#include "object.h"
#include "eval.h"

static void print_help() {
    printf("chibi-lisp - A minimal Lisp interpreter\n");
    printf("Usage: chibi-lisp [options]\n");
    printf("Options:\n");
    printf("  --debug, -d    Enable debug mode\n");
    printf("  --help, -h     Show this help message\n");
    printf("\nREPL Commands:\n");
    printf("  :quit          Exit the REPL\n");
    printf("  :mem           Show memory statistics\n");
}

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
            printf(obj->data.number ? "t" : "nil");
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
        case OBJ_CONS:
            printf("(");
            Object* current = obj;
            while (current && current->type == OBJ_CONS) {
                print_obj(current->data.cons.car);
                current = current->data.cons.cdr;
                if (current && current->type == OBJ_CONS) {
                    printf(" ");
                } else if (current && current->type != OBJ_NIL) {
                    printf(" . ");
                    print_obj(current);
                    break;
                }
            }
            printf(")");
            break;
        case OBJ_FUNCTION:
            printf("<function>");
            break;
        case OBJ_LAMBDA:
            printf("<lambda>");
            break;
        case OBJ_OPERATOR:
            printf("%s", obj_operator_name(obj));
            break;
        case OBJ_BUILTIN:
            printf("%s", obj_builtin_name(obj));
            break;
        default:
            printf("<unknown>");
            break;
    }
}

int main(int argc, char* argv[]) {
    int debug_enabled = 0;

    // ???????????
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--debug") == 0 || strcmp(argv[i], "-d") == 0) {
            debug_enabled = 1;
        } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            print_help();
            return 0;
        }
    }

    // evaluator???
    evaluator_init();
    evaluator_set_debug(debug_enabled);

    if (debug_enabled) {
        printf("Debug mode enabled.\n");
    }

    // REPL
    char line[1024];
    printf("chibi-lisp REPL. Type :quit to exit, :mem for memory stats.\n");
    while (1) {
        printf("> ");

        if (!fgets(line, sizeof(line), stdin)) {
            break;
        }

        // ?????
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }

        // REPL??????
        if (strncmp(line, ":quit", 5) == 0) {
            break;
        }
        if (strncmp(line, ":mem", 4) == 0) {
            evaluator_show_memory_stats();
            continue;
        }

        // S???
        Object* res = eval_string(line);
        print_obj(res);
        printf("\n");
    }

    evaluator_shutdown();
    return 0;
}
