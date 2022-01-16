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

typedef struct {
    void *key, *value;
    int next;
} hashmap_entry_t;

typedef struct hashmap_s {
    hashmap_entry_t *entries;
    struct hashmap_s *fallback;
    int capacity, count, next_free;
} hashmap_t;

// Allocate a new hash map
__attribute__((nonnull))
hashmap_t *new_hashmap(hashmap_t *fallback);
// Retrieve a value from a hash map (or return NULL) if not found
__attribute__((nonnull))
void *hashmap_get(hashmap_t *h, void *key);
// Remove and return a value from a hash map (or return NULL) if not found
__attribute__((nonnull(1,2)))
void *hashmap_pop(hashmap_t *h, void *key);
// Store a key/value pair in the hash map and return the previous value (if any)
__attribute__((nonnull(1,2,3)))
void *hashmap_set(hashmap_t *h, void *key, void *value);
// Get the key after the given key (or NULL to get the first key)
__attribute__((nonnull(1)))
void *hashmap_next(hashmap_t *h, void *key);
// Deallocate the memory associated with the hash map (individual entries are not freed)
__attribute__((nonnull))
void free_hashmap(hashmap_t **h);

//////////////////////////////////////////////////////
////////////////    Hash Sets     ////////////////////
//////////////////////////////////////////////////////

typedef struct {
    void *item;
    int next;
} hashset_entry_t;

typedef struct hashset_s {
    hashset_entry_t *entries;
    struct hashset_s *fallback;
    int capacity, count, next_free;
} hashset_t;

// Allocate a new hash set
__attribute__(())
hashset_t *new_hashset(hashmap_t *fallback);
// Return whether or not the hash set contains a given item
__attribute__((nonnull))
bool hashset_contains(hashset_t *h, void *item);
// Remove an item from the hash set (return true if was present or false otherwise)
__attribute__((nonnull(1,2)))
bool hashset_remove(hashset_t *h, void *item);
// Add an item to the hash set (return true if it was added or false if it was already present)
__attribute__((nonnull(1,2)))
bool hashset_add(hashset_t *h, void *item);
// Get the item after the given item (or NULL to get the first item)
__attribute__((nonnull(1)))
void *hashset_next(hashset_t *h, void *key);
// Free the memory associated with the hash set (items in the set are not freed)
__attribute__((nonnull))
void free_hashset(hashset_t **h);

//////////////////////////////////////////////////////
//////////////   String Interning   //////////////////
//////////////////////////////////////////////////////

// Intern a string into memory.
// If an equivalent string was already in the table, return that version, otherwise return a copy.
const char *str_intern(char *str);
// Transfer ownership of a dynamically allocated string to the intern table.
// If an equivalent string was already interned, free `str` and return the existing string.
// Otherwise, store `str` in the table.
const char *str_intern_transfer(char *str);
// Free all interned strings and the table used to track them.
void free_interned(void);

// vim: ts=4 sw=0 et cino=L2,l1,(0,W4,m1
