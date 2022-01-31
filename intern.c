// intern.c - C Memory Interning Library
// Copyright 2022 Bruce Hill
// Provided under the MIT license with the Commons Clause
// See included LICENSE for details.

// Memory Intern(aliz)ing Implementation
// Memory contents are hashed and stored in a table.
// When interning, if a chunk of memory is already in the table, it will be
// returned. Otherwise the original memory (or a copy of it) will be returned.
// This is useful for ensuring that each chunk of memory is only stored in
// memory once, and a single pointer to that memory can be used for
// constant-time hashing and equality comparisons. Any interned memory should
// be treated as immutable.
// See README.md for more details on the hash table impelmentation.

#include <stdlib.h>
#include <string.h>

typedef struct intern_entry_s {
    char *mem;
    size_t len;
    struct intern_entry_s *next;
} intern_entry_t;

static intern_entry_t *interned = NULL, *lastfree = NULL;
static size_t capacity = 0, count = 0;

static void intern_insert(char *mem, size_t len);

static size_t hash_mem(char *mem, size_t len)
{
    if (__builtin_expect(len == 0, 0)) return 0;
    register unsigned char *p = (unsigned char *)mem;
    register size_t h = (size_t)(*p << 7) ^ len;
    register size_t i = len > 128 ? 128 : len;
    while (i--)
        h = (1000003*h) ^ *p++;
    if (h == 0) h = 1234567;
    return h;
}

static void rehash(size_t new_size)
{
    intern_entry_t *old = interned;
    size_t old_capacity = capacity;
    interned = calloc(new_size, sizeof(intern_entry_t));
    capacity = new_size;
    count = 0;
    lastfree = &interned[new_size - 1];
    // Rehash:
    if (old) {
        for (size_t i = 0; i < old_capacity; i++)
            if (old[i].mem)
                intern_insert(old[i].mem, old[i].len);
        free(old);
    }
}

static char *lookup(char *mem, size_t len)
{
    if (capacity == 0 || !mem) return NULL;
    int i = (int)(hash_mem(mem, len) & (size_t)(capacity-1));
    for (intern_entry_t *e = &interned[i]; e && e->mem; e = e->next) {
        if (e->len == len && memcmp(mem, e->mem, len) == 0)
            return e->mem;
    }
    return NULL;
}

static void intern_insert(char *mem, size_t len)
{
    if (!mem || len == 0) return;

    // Grow the storage if necessary
    if (capacity == 0) rehash(16);
    else if ((count + 1) >= capacity)
        rehash(capacity*2);

    int i = (int)(hash_mem(mem, len) & (size_t)(capacity-1));
    intern_entry_t *collision = &interned[i];
    if (collision->mem == NULL) { // No collision
        collision->mem = mem;
        collision->len = len;
        ++count;
        return;
    }

    while (lastfree >= interned && lastfree->mem)
        --lastfree;

    int i2 = (int)(hash_mem(collision->mem, collision->len) & (size_t)(capacity-1));
    if (i2 == i) { // Collision with element in its main position
        lastfree->mem = mem;
        lastfree->len = len;
        lastfree->next = collision->next;
        collision->next = lastfree;
    } else {
        intern_entry_t *prev = &interned[i2];
        while (prev->next != collision)
            prev = prev->next;
        memcpy(lastfree, collision, sizeof(intern_entry_t));
        prev->next = lastfree;
        collision->mem = mem;
        collision->len = len;
        collision->next = NULL;
    }
    ++count;
}

// Variant that frees or transfers ownership of the memory
// to the interned memory table. Values passed in *should*
// be allocated by malloc() or similar and the return value
// should *not* be freed() other than via free_interned().
char *intern_bytes_transfer(char *mem, size_t len)
{
    if (!mem || len == 0) return NULL;
    char *dup = lookup(mem, len);
    if (dup) {
        free(mem);
        return dup;
    }
    intern_insert(mem, len);
    return mem;
}

// Variant that allocates a copy of the given memory if it
// is not already intered. If a value is passed in that is
// dynamically allocated, you are in charge of free()ing it
// yourself.
char *intern_bytes(char *mem, size_t len)
{
    if (!mem || len == 0) return NULL;
    char *dup = lookup(mem, len);
    if (dup) return dup;
    char *copy = malloc(len);
    memcpy(copy, mem, len);
    intern_insert(copy, len);
    return copy;
}

void intern_free(void)
{
    if (interned == NULL) return;
    for (size_t i = 0; i < capacity; i++) {
        if (interned[i].mem)
            free((char*)interned[i].mem);
    }
    free(interned);
    interned = NULL;
}

// vim: ts=4 sw=0 et cino=L2,l1,(0,W4,m1
