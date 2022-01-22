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

typedef struct intern_entry_s {
    char *str;
    struct intern_entry_s *next;
} intern_entry_t;

static intern_entry_t *interned = NULL, *lastfree = NULL;
static size_t capacity = 0, count = 0;

static void intern_insert(char *str);

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
            if (old[i].str)
                intern_insert(old[i].str);
        free(old);
    }
}

static char *lookup(char *str)
{
    if (capacity == 0 || !str) return NULL;
    int i = (int)(hash_str(str) & (size_t)(capacity-1));
    for (intern_entry_t *e = &interned[i]; e && e->str; e = e->next) {
        if (strcmp(str, e->str) == 0)
            return e->str;
    }
    return NULL;
}

static void intern_insert(char *str)
{
    if (!str) return;

    // Grow the storage if necessary
    if (capacity == 0) rehash(16);
    else if ((count + 1) >= capacity)
        rehash(capacity*2);

    int i = (int)(hash_str(str) & (size_t)(capacity-1));
    intern_entry_t *collision = &interned[i];
    if (collision->str == NULL) { // No collision
        collision->str = str;
        ++count;
        return;
    }

    while (lastfree >= interned && lastfree->str)
        --lastfree;

    int i2 = (int)(hash_str(collision->str) & (size_t)(capacity-1));
    if (i2 == i) { // Collision with element in its main position
        lastfree->str = str;
        lastfree->next = collision->next;
        collision->next = lastfree;
    } else {
        intern_entry_t *prev = &interned[i2];
        while (prev->next != collision)
            prev = prev->next;
        memcpy(lastfree, collision, sizeof(intern_entry_t));
        prev->next = lastfree;
        collision->str = str;
        collision->next = NULL;
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
    char *dup = lookup(str);
    if (dup) {
        free(str);
        return dup;
    }
    intern_insert(str);
    return str;
}

// Variant that allocates a copy of the given string if it
// is not already intered. If a value is passed in that is
// dynamically allocated, you are in charge of free()ing it
// yourself.
char *str_intern(char *str)
{
    if (!str) return NULL;
    char *dup = lookup(str);
    if (dup) return dup;
    intern_insert(strdup(str));
    return str;
}

void free_interned(void)
{
    if (interned == NULL) return;
    for (size_t i = 0; i < capacity; i++) {
        if (interned[i].str)
            free((char*)interned[i].str);
    }
    free(interned);
    interned = NULL;
}

// vim: ts=4 sw=0 et cino=L2,l1,(0,W4,m1
