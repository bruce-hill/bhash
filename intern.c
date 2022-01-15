#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct {
    void *key;
    int next;
} intern_entry_t;

typedef struct {
    size_t capacity, occupancy;
    intern_entry_t *entries;
    int next_free;
} intern_t;

static intern_t *interned = NULL;

static void hashset_add(intern_t *h, void *key);

static size_t hash_str(void *s)
{
    register const unsigned char *p = (const unsigned char *)s;
    register size_t h = *p << 7;
    register size_t len = 0;
    while (*p) {
        h = (1000003*h) ^ *p++;
        ++len;
        if (len >= 127) break; // Optimization: only hash the first part of the string
    }
    h ^= len;
    if (h == 0) h = 1234567;
    return h;
}

static void hashset_resize(intern_t *h, size_t new_size)
{
    intern_t tmp = *h;
    h->entries = calloc(new_size, sizeof(intern_entry_t));
    h->capacity = new_size;
    h->occupancy = 0;
    h->next_free = (int)(new_size - 1);
    if (tmp.entries) {
        // Rehash:
        for (size_t i = 0; i < tmp.capacity; i++)
            if (tmp.entries[i].key)
                hashset_add(h, tmp.entries[i].key);
        free(tmp.entries);
    }
}

void *hashset_get(intern_t *h, void *key)
{
    if (h->capacity == 0) return NULL;
    int i = (int)(hash_str(key) & (h->capacity-1));
    while (i != -1 && h->entries[i].key) {
        if (strcmp(key, h->entries[i].key) == 0)
            return h->entries[i].key;
        i = h->entries[i].next;
    }
    return NULL;
}

static void *hashset_pop(intern_t *h, void *key)
{
    if (h->capacity == 0) return NULL;
    int i = (int)(hash_str(key) & (h->capacity-1));
    int prev = i;
    while (strcmp(h->entries[i].key, key) != 0) {
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
        memset(&h->entries[i2], 0, sizeof(intern_entry_t));
        if (i2 > h->next_free) h->next_free = i2;
    } else {
        // prev->def@i ==> prev; NULL@i
        if (prev != i)
            h->entries[prev].next = -1;
        memset(&h->entries[i], 0, sizeof(intern_entry_t));
    }
    --h->occupancy;

    // Shrink the storage if it's getting real empty:
    if (h->occupancy > 16 && h->occupancy < h->capacity/3)
        hashset_resize(h, h->capacity/2);

    return ret;
}

static void hashset_add(intern_t *h, void *key)
{
    if (h->capacity == 0) hashset_resize(h, 16);

    // Grow the storage if necessary
    if ((h->occupancy + 1) >= h->capacity)
        hashset_resize(h, h->capacity*2);

    int i = (int)(hash_str(key) & (h->capacity-1));
    if (h->entries[i].key == NULL) { // No collision
        h->entries[i].key = key;
        h->entries[i].next = -1;
    } else {
        for (int j = i; j != -1; j = h->entries[j].next) {
            if (strcmp(h->entries[j].key, key) == 0)
                return;
        }

        while (h->entries[h->next_free].key) {
            if (h->next_free <= 0) h->next_free = (int)(h->capacity - 1);
            else --h->next_free;
        }
        int free = h->next_free;

        int i2 = (int)(hash_str(h->entries[i].key) & (h->capacity-1));
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

char *str_intern(char *str)
{
    if (!interned) interned = calloc(1, sizeof(intern_t));

    char *dup = hashset_get(interned, str);
    if (dup) {
        free(str);
        return dup;
    }
    hashset_add(interned, str);
    return str;
}

void free_interned(void)
{
    if (interned == NULL) return;
    for (int i = 0; i < interned->capacity; i++) {
        if (interned->entries[i].key)
            free(interned->entries[i].key);
    }
    if (interned->entries) free(interned->entries);
    free(interned);
    interned = NULL;
}

// vim: ts=4 sw=0 et cino=L2,l1,(0,W4,m1
