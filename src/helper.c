#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "helper.h"

// �r�b�g�}�b�v�̃r�b�g���Z�b�g
static inline void bitmap_set(uint8_t *bitmap, size_t bit) {
    bitmap[bit / 8] |= (1 << (bit % 8));
}

// �r�b�g�}�b�v�̃r�b�g���N���A
static inline void bitmap_clear(uint8_t *bitmap, size_t bit) {
    bitmap[bit / 8] &= ~(1 << (bit % 8));
}

// �r�b�g�}�b�v�̃r�b�g���Z�b�g����Ă��邩�m�F
static inline bool bitmap_test(const uint8_t *bitmap, size_t bit) {
    return (bitmap[bit / 8] & (1 << (bit % 8))) != 0;
}

// �r�b�g�}�b�v��S��0�ŏ�����
static inline void bitmap_clear_all(uint8_t *bitmap, size_t num_bits) {
    size_t num_bytes = (num_bits + 7) / 8;
    for (size_t i = 0; i < num_bytes; ++i) {
        bitmap[i] = 0;
    }
}

// �r�b�g�}�b�v��S��1�ŏ�����
static inline void bitmap_set_all(uint8_t *bitmap, size_t num_bits) {
    size_t num_bytes = (num_bits + 7) / 8;
    for (size_t i = 0; i < num_bytes; ++i) {
        bitmap[i] = 0xFF;
    }
}