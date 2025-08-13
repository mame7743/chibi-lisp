#ifndef __HELPER_H__
#define __HELPER_H__

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

static inline void bitmap_set(uint8_t *bitmap, size_t bit);
static inline void bitmap_clear(uint8_t *bitmap, size_t bit);
static inline bool bitmap_test(const uint8_t *bitmap, size_t bit);
static inline void bitmap_clear_all(uint8_t *bitmap, size_t num_bits);
static inline void bitmap_set_all(uint8_t *bitmap, size_t num_bits);

#endif // __HELPER_H__