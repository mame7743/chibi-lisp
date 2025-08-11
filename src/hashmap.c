#include "hashmap.h"
#include <string.h>

static uint64_t fnv_hash(const char* s, int len) {
    uint64_t hash = 14695981039346656037ULL;
    for (int i = 0; i < len; i++) {
        hash ^= (uint64_t)s[i];
        hash *= 1099511628211ULL;
    }
    return hash;
}

void hashmap_init(HashMap* map) {
    for (int i = 0; i < HASHMAP_SIZE; i++) {
        map->entries[i].key = NULL;
        map->entries[i].value = NULL;
    }
}

void* hashmap_get(HashMap* map, const char* key, size_t key_len) {
    uint64_t hash = fnv_hash(key, key_len);
    int index = hash % HASHMAP_SIZE;
    for (int i = 0; i < HASHMAP_SIZE; i++) {
        if (map->entries[index].key != NULL && 
            strncmp(map->entries[index].key, key, key_len) == 0 &&
            strlen(map->entries[index].key) == key_len) {
            return map->entries[index].value;
        }
        if (map->entries[index].key == NULL) {
            break;  // 空のスロットに到達したら終了
        }
        index = (index + 1) % HASHMAP_SIZE;  // Linear probing
    }
    return NULL;  // Not found
}

bool hashmap_set(HashMap* map, const char* key, size_t key_len, void* value) {
    uint64_t hash = fnv_hash(key, key_len);
    int index = hash % HASHMAP_SIZE;
    for (int i = 0; i < HASHMAP_SIZE; i++) {
        if (map->entries[index].key == NULL) {
            // 空いているスロットに挿入
            map->entries[index].key = key;
            map->entries[index].value = value;
            return true;
        }
        // 既存のキーを更新
        if (strncmp(map->entries[index].key, key, key_len) == 0 &&
            strlen(map->entries[index].key) == key_len) {
            map->entries[index].value = value;
            return true;
        }
        index = (index + 1) % HASHMAP_SIZE;  // Linear probing
    }
    return false;  // ハッシュマップが満杯
}
