// Hash Set Implementation
// Keys are pointers, and entries are stored in an array.
// If you want to use strings as pointers, you can intern the strings so
// that each unique string has a unique pointer.
// If you want to use numbers as pointers, you can cast them to pointers.
// The hash insertion/lookup implementation is based on Lua's tables,
// which use a chained scatter with Brent's variation.
// See README.md for more details.

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "hashset.h"

static size_t hash_pointer(void *p)
{
    size_t s = (size_t)p;
    if (s == 0) return 1234567;
    return (s >> 5) | (s << (sizeof(void*) - 5));
}

static void hashset_resize(hashset_t *h, size_t new_size)
{
    hashset_t tmp = *h;
    h->entries = calloc(new_size, sizeof(hashset_entry_t));
    h->capacity = new_size;
    h->occupancy = 0;
    h->next_free = (int)(new_size - 1);
    if (tmp.entries) {
        // Rehash:
        for (int i = 0; i < tmp.capacity; i++)
            if (tmp.entries[i].key)
                hashset_add(h, tmp.entries[i].key);
        free(tmp.entries);
    }
}

hashset_t *new_hashset(void)
{
    return calloc(1, sizeof(hashset_t));
}

void *hashset_get(hashset_t *h, void *key)
{
    if (h->capacity == 0) return NULL;
    int i = (int)(hash_pointer(key) & (h->capacity-1));
    while (i != -1 && h->entries[i].key) {
        if (key == h->entries[i].key)
            return h->entries[i].key;
        i = h->entries[i].next;
    }
    return NULL;
}

void *hashset_pop(hashset_t *h, void *key)
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

    void *ret = h->entries[i].key;
    if (h->entries[i].next != -1) {
        // @prev -> def@i -> after@i2 ->... ==> @prev -> after@i ->...; NULL@i2
        int i2 = h->entries[i].next;
        h->entries[i] = h->entries[i2];
        memset(&h->entries[i2], 0, sizeof(hashset_entry_t));
        if (i2 > h->next_free) h->next_free = i2;
    } else {
        // prev->def@i ==> prev; NULL@i
        if (prev != i)
            h->entries[prev].next = -1;
        memset(&h->entries[i], 0, sizeof(hashset_entry_t));
    }
    --h->occupancy;

    // Shrink the storage if it's getting real empty:
    if (h->occupancy > 16 && h->occupancy < h->capacity/3)
        hashset_resize(h, h->capacity/2);

    return ret;
}

void hashset_add(hashset_t *h, void *key)
{
    if (h->capacity == 0) hashset_resize(h, 16);

    // Grow the storage if necessary
    if ((h->occupancy + 1) >= h->capacity)
        hashset_resize(h, h->capacity*2);

    int i = (int)(hash_pointer(key) & (h->capacity-1));
    if (h->entries[i].key == NULL) { // No collision
        h->entries[i].key = key;
        h->entries[i].next = -1;
        return;
    } else {
        for (int j = i; j != -1; j = h->entries[j].next) {
            if (h->entries[j].key == key)
                return;
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
            h->entries[i].next = -1;
        }
    }
    ++h->occupancy;
}

void free_hash(hashset_t **h)
{
    if (*h == NULL) return;
    if ((*h)->entries) free((*h)->entries);
    free(*h);
    *h = NULL;
}
// vim: ts=4 sw=0 et cino=L2,l1,(0,W4,m1
