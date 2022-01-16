#pragma once

// Intern a string into memory.
// If an equivalent string was already in the table, return that version, otherwise return a copy.
const char *str_intern(char *str);
// Transfer ownership of a dynamically allocated string to the intern table.
// If an equivalent string was already interned, free `str` and return the existing string.
// Otherwise, store `str` in the table.
const char *str_intern_transfer(char *str);
// Free all interned strings and the table used to track them.
void free_interned(void);
