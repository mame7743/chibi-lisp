
# chibi-lisp

<p align="center">
    <img src="lambda.svg" alt="lambda" width="120">
</p>

C言語で実装された、小規模なLispインタプリタ処理系です。

## 🎯 目標

小型のLispインタプリタをC言語で自作する。
- 基本的なS式の構文と評価に対応
- `define`, `lambda`, `if`, 再帰関数, 簡単なプリミティブ関数（+ - * /）をサポート
- 最初はGCなし、再帰とリストベースのデータ構造に焦点

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

## 🧩 Lispデータ型の表現

```c
typedef enum {
    VAL_INT,
    VAL_SYMBOL,
    VAL_PAIR,
    VAL_FUNCTION,
    VAL_NIL,
} ValueType;

typedef struct Value Value;
typedef struct Env Env;

struct Value {
    ValueType type;
    union {
        int i;
        char *symbol;
        struct {
            Value *car;
            Value *cdr;
        }; // cons cell
        struct {
            Value *params;
            Value *body;
            Env *env;
        } function;
    };
};
```

---

## 🔁 評価器の動作概要

- 数値 → そのまま返す
- シンボル → 環境から検索
- リスト → 第一要素を関数とみなし、引数を評価して適用

---

## 🚀 実装ステップ

1. `Value`構造体とリスト操作関数（cons, car, cdr）の実装
2. トークナイザの実装（文字列 → トークン配列）
3. パーサの実装（トークン列 → S式）
4. 評価器の実装（`eval`）
5. 環境（変数バインディング）の実装
6. プリミティブ関数（`+`, `define`, `lambda`, `if`）の追加
7. REPLの構築

---

## 🔚 目標動作例

```lisp
> (define x 10)
> (+ x 2)
12

> (define inc (lambda (x) (+ x 1)))
> (inc 5)
6
```

---

## 🔄 今後の拡張

- 再帰・クロージャ対応
- ガベージコレクション（GC）追加
- バイトコードコンパイル
- ファイル読み込み・標準ライブラリの拡張
- Arduinoなどの組み込み環境への移植
