#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "helper.h"

// ビットマップのビットをセット
static inline void bitmap_set(uint8_t *bitmap, size_t bit) {
    bitmap[bit / 8] |= (1 << (bit % 8));
}

// ビットマップのビットをクリア
static inline void bitmap_clear(uint8_t *bitmap, size_t bit) {
    bitmap[bit / 8] &= ~(1 << (bit % 8));
}

// ビットマップのビットがセットされているか確認
static inline bool bitmap_test(const uint8_t *bitmap, size_t bit) {
    return (bitmap[bit / 8] & (1 << (bit % 8))) != 0;
}

// ビットマップを全て0で初期化
static inline void bitmap_clear_all(uint8_t *bitmap, size_t num_bits) {
    size_t num_bytes = (num_bits + 7) / 8;
    for (size_t i = 0; i < num_bytes; ++i) {
        bitmap[i] = 0;
    }
}

// ビットマップを全て1で初期化
static inline void bitmap_set_all(uint8_t *bitmap, size_t num_bits) {
    size_t num_bytes = (num_bits + 7) / 8;
    for (size_t i = 0; i < num_bytes; ++i) {
        bitmap[i] = 0xFF;
    }
}