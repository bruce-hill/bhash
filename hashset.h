#pragma once

typedef struct {
    void *key;
    int next;
} hashset_entry_t;

typedef struct {
    size_t capacity, occupancy;
    hashset_entry_t *entries;
    int next_free;
} hashset_t;

__attribute__(())
hashset_t *new_hashset(void);
__attribute__((nonnull))
void *hashset_get(hashset_t *h, void *key);
__attribute__((nonnull(1,2)))
void *hashset_pop(hashset_t *h, void *key);
__attribute__((nonnull(1,2)))
void hashset_add(hashset_t *h, void *key);
__attribute__((nonnull))
void free_hashset(hashset_t **h);

// vim: ts=4 sw=0 et cino=L2,l1,(0,W4,m1
