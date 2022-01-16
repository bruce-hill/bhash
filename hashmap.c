// hashmap.c - C Hash Map Library
// Copyright 2022 Bruce Hill
// Provided under the MIT license with the Commons Clause
// See included LICENSE for details.

// Hash Map (aka Dictionary) Implementation
// Keys are pointers, and entries are stored in an array.
// If you want to use strings as pointers, you can intern the strings so
// that each unique string has a unique pointer.
// If you want to use numbers as pointers, you can cast them to pointers.
// The hash insertion/lookup implementation is based on Lua's tables,
// which use a chained scatter with Brent's variation.
// See README.md for more details.

#include <stdlib.h>
#include <string.h>

#include "hashmap.h"

static inline size_t hash_pointer(void *p)
{
    size_t s = (size_t)p;
    if (s == 0) return 1234567;
    return (s >> 5) | (s << (sizeof(void*) - 5));
}

static void hashmap_resize(hashmap_t *h, size_t new_size)
{
    hashmap_t tmp = *h;
    h->entries = calloc(new_size, sizeof(hashmap_entry_t));
    h->capacity = new_size;
    h->count = 0;
    h->next_free = (int)(new_size - 1);
    if (tmp.entries) {
        // Rehash:
        for (int i = 0; i < tmp.capacity; i++)
            if (tmp.entries[i].key)
                hashmap_set(h, tmp.entries[i].key, tmp.entries[i].value);
        free(tmp.entries);
    }
}

hashmap_t *new_hashmap(hashmap_t *fallback)
{
    hashmap_t *h = calloc(1, sizeof(hashmap_t));
    if (h) h->fallback = fallback;
    return h;
}

void *hashmap_get(hashmap_t *h, void *key)
{
    if (h->capacity > 0) {
        int i = (int)(hash_pointer(key) & (h->capacity-1));
        while (i != -1 && h->entries[i].key) {
            if (key == h->entries[i].key)
                return h->entries[i].value;
            i = h->entries[i].next;
        }
    }
    if (h->fallback) return hashmap_get(h->fallback, key);
    return NULL;
}

void *hashmap_pop(hashmap_t *h, void *key)
{
    if (h->capacity == 0) return NULL;
    int i = (int)(hash_pointer(key) & (h->capacity-1));
    int prev = i;
    while (h->entries[i].key != key) {
        if (h->entries[i].next == -1)
            return NULL;
        prev = i;
        i = h->entries[i].next;
    }

    void *ret = h->entries[i].value;
    if (h->entries[i].next != -1) {
        // @prev -> def@i -> after@i2 ->... ==> @prev -> after@i ->...; NULL@i2
        int i2 = h->entries[i].next;
        h->entries[i] = h->entries[i2];
        memset(&h->entries[i2], 0, sizeof(hashmap_entry_t));
        if (i2 > h->next_free) h->next_free = i2;
    } else {
        // prev->def@i ==> prev; NULL@i
        if (prev != i)
            h->entries[prev].next = -1;
        memset(&h->entries[i], 0, sizeof(hashmap_entry_t));
    }
    --h->count;

    // Shrink the storage if it's getting real empty:
    if (h->count > 16 && h->count < h->capacity/3)
        hashmap_resize(h, h->capacity/2);

    return ret;
}

void *hashmap_set(hashmap_t *h, void *key, void *value)
{
    if (h->capacity == 0) hashmap_resize(h, 16);

    // Grow the storage if necessary
    if ((h->count + 1) >= h->capacity)
        hashmap_resize(h, h->capacity*2);

    int i = (int)(hash_pointer(key) & (h->capacity-1));
    if (h->entries[i].key == NULL) { // No collision
        h->entries[i].key = key;
        h->entries[i].value = value;
        h->entries[i].next = -1;
    } else {
        for (int j = i; j != -1; j = h->entries[j].next) {
            if (h->entries[j].key == key) {
                void *ret = h->entries[j].value;
                h->entries[j].key = key;
                h->entries[j].value = value;
                return ret;
            }
        }

        while (h->entries[h->next_free].key) {
            if (h->next_free <= 0) h->next_free = (int)(h->capacity - 1);
            else --h->next_free;
        }
        int free = h->next_free;

        int i2 = (int)(hash_pointer(h->entries[i].key) & (h->capacity-1));
        if (i2 == i) { // Collision with element in its main position
            // Before: colliding@i -> next
            // After:  colliding@i -> noob@free -> next
            h->entries[free].key = key;
            h->entries[free].value = value;
            h->entries[free].next = h->entries[i].next;
            h->entries[i].next = free;
        } else { // Collision with element in a chain
            int prev = i2;
            while (h->entries[prev].next != i)
                prev = h->entries[prev].next;

            // Before: _@i2 ->...-> prev@prev -> colliding@i -> next
            // After:  _@i2 ->...-> prev@prev -> colliding@free -> next; noob@i
            h->entries[prev].next = free;
            h->entries[free] = h->entries[i];
            h->entries[i].key = key;
            h->entries[i].value = value;
            h->entries[i].next = -1;
        }
    }
    ++h->count;
    return NULL;
}

void *hashmap_next(hashmap_t *h, void *key)
{
    if (h->capacity == 0) return NULL;
    int i = 0;
    if (key) {
        i = (int)(hash_pointer(key) & (h->capacity-1));
        if (!h->entries[i].key) {
            i = 0;
        } else {
            while (key != h->entries[i].key && h->entries[i].next != -1)
                i = h->entries[i].next;
            ++i;
        }
    }
    for (; i < h->capacity; i++)
        if (h->entries[i].key) return h->entries[i].key;

    return NULL;
}

void free_hashmap(hashmap_t **h)
{
    if (*h == NULL) return;
    if ((*h)->entries) free((*h)->entries);
    free(*h);
    *h = NULL;
}
// vim: ts=4 sw=0 et cino=L2,l1,(0,W4,m1
