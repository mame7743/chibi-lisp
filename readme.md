# chibi-lisp

<p align="center">
    <img src="lambda.svg" alt="lambda" width="120">
</p>

<p align="center">
    <strong>🚧 開発中 / Under Development</strong>
</p>

C言語で実装された、小規模なLispインタプリタ処理系


## 目次

- [chibi-lisp](#chibi-lisp)
  - [目次](#目次)
  - [🎯 目標](#-目標)
    - [実装状況](#実装状況)
  - [🧱 処理系の構成要素](#-処理系の構成要素)
  - [🛠 開発環境のセットアップ](#-開発環境のセットアップ)
    - [必要な環境](#必要な環境)
    - [インストール・ビルド手順](#インストールビルド手順)
      - [1. 事前準備（OS別）](#1-事前準備os別)
        - [macOS](#macos)
        - [Windows](#windows)
      - [2. 共通手順](#2-共通手順)
  - [🔚 実行例](#-実行例)
    - [基本的な算術演算と変数定義](#基本的な算術演算と変数定義)
    - [条件分岐とブール演算](#条件分岐とブール演算)
    - [リストと再帰的な処理](#リストと再帰的な処理)
  - [🔄 今後の拡張](#-今後の拡張)

## 🎯 目標

小型のLispインタプリタをC言語で自作する。
- 基本的なS式の構文と評価に対応 ⏳
- `define`, `lambda`, `if`, 再帰関数, 簡単なプリミティブ関数（+ - * /）をサポート 🚫
- 最初はGCなし、再帰とリストベースのデータ構造に焦点

### 実装状況

| 機能 | 状態 |
|------|------|
| 字句解析（Tokenizer） | ⏳ 実装中 |
| 構文解析（Parser） | 🚫 未着手 |
| 評価器（Evaluator） | 🚫 未着手 |
| 環境（Environment） | 🚫 未着手 |
| プリミティブ関数 | 🚫 未着手 |
| エラー処理 | 🚫 未着手 |
| ガベージコレクション | 🚫 未着手 |

---

## 🧱 処理系の構成要素

1. **字句解析（Tokenizer）**
    - Lispの入力文字列をトークンに分割する。
    - 例: `(+ 1 2)` → `["(", "+", "1", "2", ")"]`

2. **構文解析（Parser）**
    - トークン列からS式（リスト構造）を構築する。
    - 例: `(+ 1 2)` → `cons("+", cons(1, cons(2, nil)))`

3. **評価器（Evaluator）**
    - 構文木を評価し、結果を返す。
    - シンボルの評価、関数適用、条件分岐などを処理。

4. **環境（Environment）**
    - シンボル（変数や関数）のバインディングを保持。
    - 連想リストやハッシュテーブルで実装。

5. **REPL（Read Eval Print Loop）**
    - 対話型シェル。
    - 入力 → 評価 → 出力 を繰り返す。

6. **プリミティブ関数**
    - C言語で定義された基本操作。
    - `+`, `-`, `car`, `cdr`, `cons`, `eq?` など。

---

## 🛠 開発環境のセットアップ

### 必要な環境

- C コンパイラ（gcc、clang、またはMSVC）
- CMake（バージョン3.10以上）
- Git

### インストール・ビルド手順

#### 1. 事前準備（OS別）

##### macOS

1. Homebrewのインストール（未導入の場合）
```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

2. 必要なツールのインストール
```bash
brew install cmake ninja
xcode-select --install  # Command Line Toolsのインストール
```

##### Windows

1. [MSYS2](https://www.msys2.org/)をインストール
2. MSYS2から必要なパッケージをインストール：
```bash
# MSYS2のターミナルで実行
pacman -Syu
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake mingw-w64-x86_64-ninja git
```
3. `C:\msys64\mingw64\bin`をシステム環境変数PATHに追加

#### 2. 共通手順

1. ソースコードの取得
```bash
git clone https://github.com/mame7743/chibi-lisp.git
cd chibi-lisp
git submodule update --init --recursive
```

2. ビルドとテスト

```bash
# ビルドディレクトリの作成と移動
mkdir build && cd build

# ビルド
cmake -G Ninja .. && ninja

# テストの実行
ctest

# REPLの起動
./chibi-lisp[.exe]
```

注意事項：
- Windowsでは、全ての操作をMSYS2のMingw64シェルで実行してください
- `[.exe]`はWindowsの場合のみ付加してください
- 個別のテストを実行する場合は以下のコマンドを使用します：
  ```bash
  ./test_tokenizer[.exe]  # Tokenizerのテスト
  ./test_parser[.exe]     # Parserのテスト
  ./test_env[.exe]        # 変数・ラムダのテスト
  ```

---

## 🔚 実行例

### 基本的な算術演算と変数定義
```lisp
; 算術演算
> (+ 1 2 3 4)
10
> (* 2 3 4)
24
> (/ (- 100 20) 5)
16

; 変数の定義と使用
> (define radius 5)
> (define pi 3.14159)
> (* pi (* radius radius))  ; 円の面積
78.53975
```

### 条件分岐とブール演算
```lisp
; 基本的な条件分岐
> (if (> 5 3) 
      'true 
      'false)
true

; 複合条件
> (define x 10)
> (if (and (> x 5) 
           (<= x 10))
      "範囲内です"
      "範囲外です")
"範囲内です"
```

### リストと再帰的な処理
```lisp
; リスト操作
> (define lst '(1 2 3 4 5))
> (car lst)     ; 先頭要素の取得
1
> (cdr lst)     ; 残りのリスト
(2 3 4 5)
> (cons 0 lst)  ; 新しいリスト作成
(0 1 2 3 4 5)

; 再帰関数の定義と使用
> (define sum
    (lambda (lst)
      (if (null? lst)
          0
          (+ (car lst) 
             (sum (cdr lst))))))

> (sum lst)  ; リストの要素の合計
15

; 階乗計算
> (define factorial
    (lambda (n)
      (if (= n 0)
          1
          (* n (factorial (- n 1))))))

> (factorial 5)
120
```

---

## 🔄 今後の拡張

- 再帰・クロージャ対応
- ガベージコレクション（GC）追加
- バイトコードコンパイル
- ファイル読み込み・標準ライブラリの拡張
- Arduinoなどの組み込み環境への移植