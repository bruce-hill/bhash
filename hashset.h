// hashset.h - C Hash Set Library
// Copyright 2022 Bruce Hill
// Provided under the MIT license with the Commons Clause
// See included LICENSE for details.

#pragma once

#include <stdbool.h>

typedef struct {
    void *item;
    int next;
} hashset_entry_t;

typedef struct {
    hashset_entry_t *entries;
    int capacity, occupancy, next_free;
} hashset_t;

// Allocate a new hash set
__attribute__(())
hashset_t *new_hashset(void);
// Return whether or not the hash set contains a given item
__attribute__((nonnull))
bool hashset_contains(hashset_t *h, void *item);
// Remove an item from the hash set (return true if was present or false otherwise)
__attribute__((nonnull(1,2)))
bool hashset_remove(hashset_t *h, void *item);
// Add an item to the hash set (return true if it was added or false if it was already present)
__attribute__((nonnull(1,2)))
bool hashset_add(hashset_t *h, void *item);
// Free the memory associated with the hash set (items in the set are not freed)
__attribute__((nonnull))
void free_hashset(hashset_t **h);

// vim: ts=4 sw=0 et cino=L2,l1,(0,W4,m1
