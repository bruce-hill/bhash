// intern.c - C String Interning Library
// Copyright 2022 Bruce Hill
// Provided under the MIT license with the Commons Clause
// See included LICENSE for details.

// String Intern(aliz)ing Implementation
// Strings are hashed and stored in a table.
// When interning, if a string is already in the table, it will be
// returned. Otherwise the original string (or a copy of it) will
// be returned. This is useful for ensuring that each string is
// only stored in memory once, and string equality and hashing can
// be performed on the pointers of the strings. These strings should
// be treated as immutable.
// See README.md for more details on the hash table impelmentation.

#include <string.h>
#include <stdlib.h>

typedef struct {
    char *str;
    int next;
} intern_entry_t;

static intern_entry_t *interned = NULL;
static int capacity = 0, count = 0, next_free = 0;

static void hashset_add(char *str);

static size_t hash_str(char *s)
{
    register unsigned char *p = (unsigned char *)s;
    register size_t h = (size_t)(*p << 7);
    register size_t len = 0;
    while (*p) {
        h = (1000003*h) ^ *p++;
        ++len;
        if (len >= 127) {
            len += strlen((char*)p+1);
            break; // Optimization: only hash the first part of the string
        }
    }
    h ^= len;
    if (h == 0) h = 1234567;
    return h;
}

static void hashset_resize(int new_size)
{
    intern_entry_t *old = interned;
    int old_capacity = capacity;
    interned = calloc((size_t)new_size, sizeof(intern_entry_t));
    capacity = new_size;
    count = 0;
    next_free = new_size - 1;
    // Rehash:
    if (old) {
        for (int i = 0; i < old_capacity; i++)
            if (old[i].str)
                hashset_add(old[i].str);
        free(old);
    }
}

static char *hashset_get(char *str)
{
    if (capacity == 0 || !str) return NULL;
    int i = (int)(hash_str(str) & (size_t)(capacity-1));
    while (i != -1 && interned[i].str) {
        if (strcmp(str, interned[i].str) == 0)
            return interned[i].str;
        i = interned[i].next;
    }
    return NULL;
}

static void hashset_add(char *str)
{
    if (!str) return;

    // Grow the storage if necessary
    if (capacity == 0) hashset_resize(16);
    else if ((count + 1) >= capacity)
        hashset_resize(capacity*2);

    int i = (int)(hash_str(str) & (size_t)(capacity-1));
    if (interned[i].str == NULL) { // No collision
        interned[i].str = str;
        interned[i].next = -1;
    } else {
        for (int j = i; j != -1; j = interned[j].next) {
            if (strcmp(interned[j].str, str) == 0)
                return;
        }

        while (interned[next_free].str) {
            if (next_free <= 0) next_free = (int)(capacity - 1);
            else --next_free;
        }
        int free = next_free;

        int i2 = (int)(hash_str(interned[i].str) & (size_t)(capacity-1));
        if (i2 == i) { // Collision with element in its main position
            // Before: colliding@i -> next
            // After:  colliding@i -> noob@free -> next
            interned[free].str = str;
            interned[free].next = interned[i].next;
            interned[i].next = free;
        } else { // Collision with element in a chain
            int prev = i2;
            while (interned[prev].next != i)
                prev = interned[prev].next;

            // Before: _@i2 ->...-> prev@prev -> colliding@i -> next
            // After:  _@i2 ->...-> prev@prev -> colliding@free -> next; noob@i
            interned[prev].next = free;
            interned[free] = interned[i];
            interned[i].str = str;
            interned[i].next = -1;
        }
    }
    ++count;
}

// Variant that frees or transfers ownership of the string
// to the interned string table. Values passed in *should*
// be allocated by malloc() or similar and the return value
// should *not* be freed() other than via free_interned().
char *str_intern_transfer(char *str)
{
    if (!str) return NULL;
    char *dup = hashset_get(str);
    if (dup) {
        free(str);
        return dup;
    }
    hashset_add(str);
    return str;
}

// Variant that allocates a copy of the given string if it
// is not already intered. If a value is passed in that is
// dynamically allocated, you are in charge of free()ing it
// yourself.
char *str_intern(char *str)
{
    if (!str) return NULL;
    char *dup = hashset_get(str);
    if (dup) return dup;
    str = strdup(str);
    hashset_add(str);
    return str;
}

void free_interned(void)
{
    if (interned == NULL) return;
    for (int i = 0; i < capacity; i++) {
        if (interned[i].str)
            free((char*)interned[i].str);
    }
    free(interned);
    interned = NULL;
}

// vim: ts=4 sw=0 et cino=L2,l1,(0,W4,m1
