#ifndef __HASHMAP_H__
#define __HASHMAP_H__

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define HASHMAP_SIZE 1024

typedef struct {
    const char* key;
    void* value;
} HashMapEntry;

typedef struct {
    HashMapEntry entries[HASHMAP_SIZE];
} HashMap;

// 関数宣言
void hashmap_init(HashMap* map);
void* hashmap_get(HashMap* map, const char* key, size_t key_len);
bool hashmap_set(HashMap* map, const char* key, size_t key_len, void* value);

#endif // __HASHMAP_H__