# Hashing Library

This library provides some basic tools for working with interned strings, hash
maps, and hash sets. The hash maps/sets both work with pointers, so it's
recommended to use interned strings if you want to use strings as hash keys.

The library is split into three separate modules that do not depend on each
other and are each quite small and easy to drop into a project as standalones.

## Hash Maps

[hashmap.h](hashmap.h) defines a hash map (aka Dict) that maps arbitrary
pointers to other pointers. The API is as follows:

```c
hashmap_t *new_hashmap(void);
void *hashmap_get(hashmap_t *h, void *key);
void *hashmap_pop(hashmap_t *h, void *key);
void *hashmap_set(hashmap_t *h, void *key, void *value);
void free_hashmap(hashmap_t **h);
```

## Hash Sets

[hashset.h](hashset.h) defines hash sets, which store unordered collections of 
deduplicated pointers. The API is as follows:

```c
hashset_t *new_hashset(void);
bool hashset_contains(hashset_t *h, void *item);
bool hashset_remove(hashset_t *h, void *item);
bool hashset_add(hashset_t *h, void *item);
void free_hashset(hashset_t **h);
```

## String Interning

[intern.h](intern.h) defines a simple API for [interning
strings](https://en.wikipedia.org/wiki/String_interning). The API is as follows:

```c
const char *str_intern(char *str);
const char *str_intern_transfer(char *str);
void free_interned(void);
```

Generally speaking `str_intern_transfer()` should be used for dynamically
allocated strings and `str_intern()` should be used if you do not want the
library to potentially call `free()` on any values you pass in.

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
