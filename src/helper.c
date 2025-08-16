// helper.c - Low-level bitmap operations
#include "helper.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

// ビットマップのビットをセット
void bitmap_set(uint8_t *bitmap, size_t bit) {
    uint8_t mask = 1 << (bit % 8);
    size_t byte_index = bit / 8;
    bitmap[byte_index] |= mask;
}

// ビットマップのビットをクリア
void bitmap_clear(uint8_t *bitmap, size_t bit) {
    uint8_t mask = 1 << (bit % 8);
    size_t byte_index = bit / 8;
    bitmap[byte_index] &= ~mask;
}

// ビットマップのビットをテスト
bool bitmap_test(const uint8_t *bitmap, size_t bit) {
    uint8_t mask = 1 << (bit % 8);
    size_t byte_index = bit / 8;
    return (bitmap[byte_index] & mask) != 0;
}

// ビットマップ全体をクリア
void bitmap_clear_all(uint8_t *bitmap, size_t num_bits) {
    size_t num_bytes = (num_bits + 7) / 8;
    memset(bitmap, 0x00, num_bytes);
}

// ビットマップ全体をセット
void bitmap_set_all(uint8_t *bitmap, size_t num_bits) {
    size_t num_bytes = (num_bits + 7) / 8;
    memset(bitmap, 0xFF, num_bytes);

    // 最後のバイトの未使用ビットをクリア
    size_t extra_bits = num_bits % 8;
    if (extra_bits != 0) {
        uint8_t mask = (1 << extra_bits) - 1;
        bitmap[num_bytes - 1] &= mask;
    }
}

// 互換API実体
void bitmap_set_bit(uint8_t *bitmap, size_t bit) { bitmap_set(bitmap, bit); }
void bitmap_clear_bit(uint8_t *bitmap, size_t bit) { bitmap_clear(bitmap, bit); }
bool bitmap_get_bit(const uint8_t *bitmap, size_t bit) { return bitmap_test(bitmap, bit); }
