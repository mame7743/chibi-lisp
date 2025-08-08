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
    src/repl.c)

# メインライブラリの作成
add_library(chibi-lisp-lib ${LISP_SOURCES})
target_include_directories(chibi-lisp-lib PUBLIC src)

# メイン実行ファイル
add_executable(chibi-lisp src/main.c)
target_link_libraries(chibi-lisp PRIVATE chibi-lisp-lib)

# Unityフレームワークのインクルードパス
include_directories(lib/unity)

# テスト実行ファイル
add_executable(test_tokenizer test/test_tokenizer.c lib/unity/unity.c)
target_link_libraries(test_tokenizer PRIVATE chibi-lisp-lib)
target_include_directories(test_tokenizer PRIVATE lib/unity)

add_executable(test_parser test/test_parser.c lib/unity/unity.c)
target_link_libraries(test_parser PRIVATE chibi-lisp-lib)
target_include_directories(test_parser PRIVATE lib/unity)

# 変数・ラムダテスト
add_executable(test_env test/test_env.c lib/unity/unity.c)
target_link_libraries(test_env PRIVATE chibi-lisp-lib)
target_include_directories(test_env PRIVATE lib/unity)

# テストの登録
add_test(NAME test_tokenizer COMMAND test_tokenizer)
add_test(NAME test_parser COMMAND test_parser)
add_test(NAME test_env COMMAND test_env)