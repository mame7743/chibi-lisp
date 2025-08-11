cmake_minimum_required(VERSION 3.10)
project(chibi-lisp)

enable_testing()

# ソースファイルのリスト
set(LISP_SOURCES
    src/tokenizer.c
    src/parser.c
    src/eval.c
    src/env.c
    src/value.c
    src/primitive.c
    src/repl.c
    src/heap.c
    src/hashmap.c
    src/lisp_heap.c)

# メインライブラリの作成
add_library(chibi-lisp-lib ${LISP_SOURCES})
target_include_directories(chibi-lisp-lib PUBLIC src)

# メイン実行ファイル
add_executable(chibi-lisp src/main.c)
target_link_libraries(chibi-lisp PRIVATE chibi-lisp-lib)

# Unityフレームワークのライブラリ作成
add_library(unity lib/unity/src/unity.c)
target_include_directories(unity PUBLIC lib/unity/src)

# テスト実行ファイル
add_executable(test_tokenizer test/test_tokenizer.c)
target_link_libraries(test_tokenizer PRIVATE chibi-lisp-lib unity)

add_executable(test_parser test/test_parser.c)
target_link_libraries(test_parser PRIVATE chibi-lisp-lib unity)

# 変数・ラムダテスト
add_executable(test_env test/test_env.c)
target_link_libraries(test_env PRIVATE chibi-lisp-lib unity)

# ヒープテスト
add_executable(test_heap test/test_heap.c)
target_link_libraries(test_heap PRIVATE chibi-lisp-lib unity)

# ハッシュマップテスト
add_executable(test_hashmap test/test_hashmap.c)
target_link_libraries(test_hashmap PRIVATE chibi-lisp-lib unity)

# Lispヒープテスト
add_executable(test_lisp_heap test/test_lisp_heap.c)
target_link_libraries(test_lisp_heap PRIVATE chibi-lisp-lib unity)
target_include_directories(test_lisp_heap PRIVATE src)

# Lispヒープ拡張テスト
add_executable(test_lisp_heap_extended test/test_lisp_heap_extended.c)
target_link_libraries(test_lisp_heap_extended PRIVATE chibi-lisp-lib unity)
target_include_directories(test_lisp_heap_extended PRIVATE src)

# テストの登録
add_test(NAME test_tokenizer COMMAND test_tokenizer)
add_test(NAME test_parser COMMAND test_parser)
add_test(NAME test_env COMMAND test_env)
add_test(NAME test_heap COMMAND test_heap)
add_test(NAME test_hashmap COMMAND test_hashmap)
add_test(NAME test_lisp_heap COMMAND test_lisp_heap)
add_test(NAME test_lisp_heap_extended COMMAND test_lisp_heap_extended)