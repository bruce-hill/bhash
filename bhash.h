// bhash.h - C Hashing Library
// Copyright 2022 Bruce Hill
// Provided under the MIT license with the Commons Clause
// See included LICENSE for details.

// This file is a combination of the files hashmap.h, hashset.h, and intern.h

#pragma once

#include <stdbool.h>

//////////////////////////////////////////////////////
////////////////    Hash Maps     ////////////////////
//////////////////////////////////////////////////////

typedef struct hashmap_entry_s {
    void *key, *value;
    struct hashmap_entry_s *next;
} hashmap_entry_t;

typedef struct hashmap_s {
    hashmap_entry_t *entries, *lastfree;
    struct hashmap_s *fallback;
    int capacity, count;
} hashmap_t;

// Allocate a new hash map
__attribute__((warn_unused_result))
hashmap_t *hashmap_new(hashmap_t *fallback);
// Retrieve a value from a hash map (or return NULL) if not found
__attribute__((nonnull,warn_unused_result))
void *hashmap_get(hashmap_t *h, void *key);
// Store a key/value pair in the hash map and return the previous value (if any)
__attribute__((nonnull(1,2)))
void *hashmap_set(hashmap_t *h, void *key, void *value);
// Get the key after the given key (or NULL to get the first key)
__attribute__((nonnull(1),warn_unused_result))
void *hashmap_next(hashmap_t *h, void *key);
// Deallocate the memory associated with the hash map (individual entries are not freed)
__attribute__((nonnull))
void hashmap_free(hashmap_t **h);

#define hashmap_pop(h, key) hashmap_set(h, key, NULL)

//////////////////////////////////////////////////////
//////////////   String Interning   //////////////////
//////////////////////////////////////////////////////

// Intern a string into memory.
// If an equivalent string was already in the table, return that version, otherwise return a copy.
__attribute__((warn_unused_result))
char *intern_str(char *str);
// Transfer ownership of a dynamically allocated string to the intern table.
// If an equivalent string was already interned, free `str` and return the existing string.
// Otherwise, store `str` in the table.
__attribute__((warn_unused_result))
char *intern_str_transfer(char *str);
// Free all interned strings and the table used to track them.
void intern_free(void);

// vim: ts=4 sw=0 et cino=L2,l1,(0,W4,m1
