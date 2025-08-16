#ifndef __HELPER_H__
#define __HELPER_H__

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void bitmap_set(uint8_t *bitmap, size_t bit);
void bitmap_clear(uint8_t *bitmap, size_t bit);
bool bitmap_test(const uint8_t *bitmap, size_t bit);
void bitmap_clear_all(uint8_t *bitmap, size_t num_bits);
void bitmap_set_all(uint8_t *bitmap, size_t num_bits);

// 互換API（以前のテストコード用）: リンク時に解決される通常関数として提供
void bitmap_set_bit(uint8_t *bitmap, size_t bit);
void bitmap_clear_bit(uint8_t *bitmap, size_t bit);
bool bitmap_get_bit(const uint8_t *bitmap, size_t bit);

#endif // __HELPER_H__