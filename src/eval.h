// eval.h
// 評価器のインターフェース定義。

#ifndef EVAL_H
#define EVAL_H

#include "object.h"

#ifdef __cplusplus
extern "C" {
#endif

// 初期化/終了
void evaluator_init(void);
void evaluator_shutdown(void);

// デバッグモード制御
void evaluator_set_debug(bool enable);

// メモリ統計表示
void evaluator_show_memory_stats(void);

// 文字列のS式を評価して結果のObjectを返す
Object* eval_string(const char* src);

#ifdef __cplusplus
}
#endif

#endif // EVAL_H
