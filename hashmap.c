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

#include "bhash.h"

static inline size_t hash_pointer(void *p)
{
    size_t s = (size_t)p;
    if (s == 0) return 1234567;
    return (s >> 5) | (s << (sizeof(void*) - 5));
}

static void hashmap_resize(hashmap_t *h, int new_size)
{
    hashmap_t old = *h;
    h->entries = (h->allocator ? h->allocator : calloc)((size_t)new_size, sizeof(hashmap_entry_t));
    h->capacity = new_size;
    h->lastfree = &h->entries[new_size - 1];
    if (old.entries) {
        // Rehash:
        for (int i = 0; i < old.capacity; i++)
            if (old.entries[i].key)
                (void)hashmap_set(h, old.entries[i].key, old.entries[i].value);
        (h->freer ? h->freer : free)(old.entries);
    }
}

hashmap_t *hashmap_new(hashmap_t *fallback)
{
    hashmap_t *h = calloc(1, sizeof(hashmap_t));
    if (!h) return h;
    h->fallback = fallback;
    return h;
}

hashmap_t *hashmap_new_alloc(void *(*allocator)(size_t,size_t), void (*freer)(void*), hashmap_t *fallback)
{
    hashmap_t *h = allocator(1, sizeof(hashmap_t));
    if (!h) return h;
    h->fallback = fallback;
    h->allocator = allocator;
    h->freer = freer;
    return h;
}

size_t hashmap_length(hashmap_t *h)
{
    return (size_t)h->count;
}

void hashmap_clear(hashmap_t *h)
{
    if (h->capacity == 0) return;
    (h->freer ? h->freer : free)(h->entries);
    h->entries = NULL;
    h->lastfree = NULL;
    h->capacity = 0;
    h->count = 0;
}

void *hashmap_get(hashmap_t *h, void *key)
{
    if (h->capacity > 0) {
        int i = (int)(hash_pointer(key) & (size_t)(h->capacity-1));
        for (hashmap_entry_t *e = &h->entries[i]; e && e->key; e = e->next) {
            if (e->key == key)
                return e->value;
        }
    }
    if (h->fallback) return hashmap_get(h->fallback, key);
    return NULL;
}

void *hashmap_set(hashmap_t *h, void *key, void *value)
{
    if (key == NULL) return NULL;

    if (h->capacity == 0) hashmap_resize(h, 16);

  retry:;
    int i = (int)(hash_pointer(key) & (size_t)(h->capacity-1));
    hashmap_entry_t *collision = &h->entries[i];
    if (!collision->key) { // Found empty slot
        if (!value) return NULL;
        collision->key = key;
        collision->value = value;
        collision->next = NULL;
        ++h->count;
        return NULL;
    }

    int i2 = (int)(hash_pointer(collision->key) & (size_t)(h->capacity-1));
    if (i2 == i) { // Hit a node in the correct place
        // Check for update to existing key:
        for (hashmap_entry_t *e = collision; e && e->key; e = e->next) {
            if (e->key == key) { // Update value
                void *old_value = e->value;
                h->count += (value ? 1 : 0) + (old_value ? -1 : 0);
                e->value = value;
                return old_value;
            }
        }
    }

    // Find a free space to insert:
    while (h->lastfree->key && h->lastfree >= h->entries)
        --h->lastfree;

    // No spaces left, gotta resize and try again:
    if (h->lastfree < h->entries) {
        int newsize = h->capacity;
        if (h->count + 1 > newsize) newsize *= 2;
        else if (h->count + 1 <= newsize/2) newsize /= 2;
        hashmap_resize(h, newsize);
        goto retry;
    }

    if (i2 == i) {
        // Put new node in a free slot
        h->lastfree->key = key;
        h->lastfree->value = value;
        // Put it between the colliding node and the second node in the chain
        h->lastfree->next = collision->next;
        collision->next = h->lastfree;
    } else { // Hit the middle of a chain for some other hash value
        // Rearrange from prevcollision..collision@i..nextcollision, NULL@nextfree
        // to: (key:value)@i, prevcollision..collision@nextfree..nextcollision
        hashmap_entry_t *prev = &h->entries[i2];
        while (prev->next != collision)
            prev = prev->next;

        // Scootch collider to new space
        memcpy(h->lastfree, collision, sizeof(hashmap_entry_t));
        prev->next = h->lastfree;

        collision->key = key;
        collision->value = value;
        collision->next = NULL;
    }
    ++h->count;
    return NULL;
}

void *hashmap_next(hashmap_t *h, void *key)
{
    if (h->capacity == 0) return NULL;
    hashmap_entry_t *e = &h->entries[0];
    if (key) {
        // Find entry in the hash table
        int i = (int)(hash_pointer(key) & (size_t)(h->capacity-1));
        e = &h->entries[i];
        if (!e->key) return NULL;
        while (e && e->key != key)
            e = e->next;
        if (!e) return NULL;
        // Then start looking for the next free entry after it
        ++e;
    }
    for (; e < &h->entries[h->capacity]; e++)
        if (e->key && e->value) return e->key;

    return NULL;
}

void hashmap_free(hashmap_t **h)
{
    if (*h == NULL) return;
    if ((*h)->entries) ((*h)->freer ? (*h)->freer : free)((*h)->entries);
    ((*h)->freer ? (*h)->freer : free)(*h);
    *h = NULL;
}
// vim: ts=4 sw=0 et cino=L2,l1,(0,W4,m1
