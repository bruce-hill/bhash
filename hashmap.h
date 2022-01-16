// hashmap.h - C Hash Map Library
// Copyright 2022 Bruce Hill
// Provided under the MIT license with the Commons Clause
// See included LICENSE for details.

#pragma once

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

// Allocate a new hash map
__attribute__((nonnull))
hashmap_t *new_hashmap(void);
// Retrieve a value from a hash map (or return NULL) if not found
__attribute__((nonnull))
void *hashmap_get(hashmap_t *h, void *key);
// Remove and return a value from a hash map (or return NULL) if not found
__attribute__((nonnull(1,2)))
void *hashmap_pop(hashmap_t *h, void *key);
// Store a key/value pair in the hash map and return the previous value (if any)
__attribute__((nonnull(1,2,3)))
void *hashmap_set(hashmap_t *h, void *key, void *value);
// Deallocate the memory associated with the hash map (individual entries are not freed)
__attribute__((nonnull))
void free_hashmap(hashmap_t **h);

// vim: ts=4 sw=0 et cino=L2,l1,(0,W4,m1
