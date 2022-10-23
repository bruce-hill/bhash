// bhash.h - C Hashing Library
// Copyright 2022 Bruce Hill
// Provided under the MIT license with the Commons Clause
// See included LICENSE for details.

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

//////////////////////////////////////////////////////
////////////////    Hash Maps     ////////////////////
//////////////////////////////////////////////////////

typedef struct hashmap_entry_s {
    const void *key;
    void *value;
    struct hashmap_entry_s *next;
} hashmap_entry_t;

typedef struct hashmap_s {
    hashmap_entry_t *entries, *lastfree;
    struct hashmap_s *fallback;
    int capacity, count;
} hashmap_t;

// Set custom allocator/freer
__attribute__((nonnull(1)))
void hashmap_set_allocator(void *(*alloc)(size_t), void (*free)(void*));

// Allocate a new hash map
__attribute__((warn_unused_result))
hashmap_t *hashmap_new(void);
// Copy a hash map
__attribute__((warn_unused_result))
hashmap_t *hashmap_copy(hashmap_t *h);
// Get a hash map's length
__attribute__((nonnull))
size_t hashmap_length(hashmap_t *h);
// Retrieve a value from a hash map (or return NULL) if not found
__attribute__((nonnull,warn_unused_result))
void *hashmap_get(hashmap_t *h, const void *key);
// Store a key/value pair in the hash map and return the previous value (if any)
__attribute__((nonnull(1,2)))
void *hashmap_set(hashmap_t *h, const void *key, const void *value);
// Get the key after the given key (or NULL to get the first key)
__attribute__((nonnull(1),warn_unused_result))
const void *hashmap_next(hashmap_t *h, const void *key);
// Clear out all entries in the hashmap
__attribute__((nonnull))
void hashmap_clear(hashmap_t *h);
// Deallocate the memory associated with the hash map (individual entries are not freed)
__attribute__((nonnull))
void hashmap_free(hashmap_t **h);

#define hashmap_pop(h, key) hashmap_set(h, key, NULL)

// vim: ts=4 sw=0 et cino=L2,l1,(0,W4,m1
