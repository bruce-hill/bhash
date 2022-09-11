# Basic Hashing (bhash) Library

This library provides a basic but performant hash map implementation for C. The
hash maps work with pointers, so it's recommended to use interned strings if
you want to use strings as hash keys. My other library
[intern](https://github.com/bruce-hill/intern/) was designed alongside this
library and intended to be used together, although it's not required.

## Installation

```sh
make && sudo make install
```

## Usage

The library is split into three separate modules that do not depend on each
other and are each quite small and easy to drop into a project as standalones.
The simplest option is definitely to throw in a copy of the `.h`/`.c` files you
need and compile your program with those.

See [example.c](example.c) for some basic examples of how to use the library
(it can be compiled with `make example`).

## API

### Hash Maps

[hashmap.h](hashmap.h) defines a hash map (aka Dict) that maps arbitrary
pointers to other pointers. The API is as follows:

```c
hashmap_t *hashmap_new(void)
hashmap_t *hashmap_copy(hashmap_t *h)
size_t hashmap_length(hashmap_t *h)
void *hashmap_get(hashmap_t *h, void *key)
void *hashmap_set(hashmap_t *h, void *key, void *value)
void *hashmap_next(hashmap_t *h, void *key)
void hashmap_clear(hashmap_t *h)
void hashmap_free(hashmap_t **h)
```

Missing values are represented as `NULL`, so you can remove entries by setting
the value to `NULL`. If you need to store `NULL` in your table, use a sentinel
value instead, or XOR values with a sentinel before/after storing.

Additionally, you can set a custom allocator for hash map allocations:

```c
void hashmap_set_allocator(void *(*alloc)(size_t), void (*free)(void*))
```

# Hash Table Implementation

The hash table implementation used here is based on Lua's tables. It uses a
chained scatter with Brent's variant. This is a highly performant way to
implement hash tables that has extremely high space efficiency while still
maintaining excellent speed.

When values are inserted into the hash table, if there is a "genuine"
collision, where two values have the same hash value (modulo the table size),
then one of the two entries is displaced into the first available free spot in
the table, and the other entry remembers the index into with the displaced
entry was stored. Upon lookup, the code first checks the table according to the
hash value (modulo the table size), and if it fails to find the value, it looks
to the index of the next displaced entry and continues doing so until it either
finds the key it was looking for, or finds an entry that has no displaced
entries following it. This is the so-called "chained scatter."

Sometimes, on insertion, a hash value may experience a collision with one of
the previously displaced entries. In such cases, the displaced entry is
re-displaced to a new empty slot and the inserted entry is placed in the slot
that corresponds to its hash value. The previous entry to the displaced entry
is then looked up (by traversing the hash chain from the start) and updated to
reflect the new location of the re-displaced entry.

In practice, this means that hash entries only need to store their key, value,
and a single integer, so there is little overhead. All hash entries are stored
in a single bulk-allocated table, so memory fragmentation is minimal and cache
coherence is high. Finally, because the entries each store the index of
colliding entries directly, there is no need to probe the table to find
colliding entries and the table can operate efficiently even up to 100%
occupancy. The only downside is that slightly more work needs to be done upon
hash insertion when a collision occurs with a displaced entry.
