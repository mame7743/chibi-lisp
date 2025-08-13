cmake_minimum_required(VERSION 3.10)
project(chibi-lisp)

enable_testing()

# ソースファイルのリスト
set(LISP_SOURCES
    src/object.c
    src/object_pool.c
    src/gc.c
    src/tokenizer.c
    src/parser.c
    src/eval.c
    src/heap.c)

# メインライブラリの作成
add_library(chibi-lisp-lib ${LISP_SOURCES})
target_include_directories(chibi-lisp-lib PUBLIC src)

# REPL 実行ファイル
add_executable(repl src/repl.c)
target_link_libraries(repl PRIVATE chibi-lisp-lib)
target_include_directories(repl PRIVATE src)

# メイン実行ファイル（必要なら維持）
add_executable(chibi-lisp src/main.c)
target_link_libraries(chibi-lisp PRIVATE chibi-lisp-lib)

# Unityフレームワークのライブラリ作成
add_library(unity lib/unity/src/unity.c)
target_include_directories(unity PUBLIC lib/unity/src)

# テスト実行ファイル
add_executable(test_object_system test/test_object_system.c)
target_link_libraries(test_object_system PRIVATE chibi-lisp-lib unity)
target_include_directories(test_object_system PRIVATE src)

add_executable(test_tokenizer test/test_tokenizer.c)
target_link_libraries(test_tokenizer PRIVATE chibi-lisp-lib unity)

add_executable(test_parser test/test_parser.c)
target_link_libraries(test_parser PRIVATE chibi-lisp-lib unity)

# レキサテスト
add_executable(test_lexer test/test_lexer.c)
target_link_libraries(test_lexer PRIVATE chibi-lisp-lib unity)

# ヒープテスト
add_executable(test_heap test/test_heap.c)
target_link_libraries(test_heap PRIVATE chibi-lisp-lib unity)

# テストの登録
add_test(NAME test_object_system COMMAND test_object_system)
add_test(NAME test_tokenizer COMMAND test_tokenizer)
add_test(NAME test_parser COMMAND test_parser)
add_test(NAME test_lexer COMMAND test_lexer)
add_test(NAME test_heap COMMAND test_heap)