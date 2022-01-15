#ifndef FILE__HASHTABLE_H
#define FILE__HASHTABLE_H

#include <stdbool.h>
#include <string.h>

typedef struct {
    void *key, *value;
    int next;
} hashmap_entry_t;

typedef struct {
    hashmap_entry_t *entries;
    int capacity, occupancy, next_free;
} hashmap_t;

__attribute__((nonnull))
hashmap_t *new_hashmap(void);
__attribute__((nonnull))
void *hashmap_get(hashmap_t *h, void *key);
__attribute__((nonnull(1,2)))
void *hashmap_pop(hashmap_t *h, void *key);
__attribute__((nonnull(1,2,3)))
void *hashmap_set(hashmap_t *h, void *key, void *value);
__attribute__((nonnull))
void free_hashmap(hashmap_t **h);

#endif
// vim: ts=4 sw=0 et cino=L2,l1,(0,W4,m1
