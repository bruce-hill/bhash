// intern.h - C String Interning Library
// Copyright 2022 Bruce Hill
// Provided under the MIT license with the Commons Clause
// See included LICENSE for details.

#pragma once

// Intern a string into memory.
// If an equivalent string was already in the table, return that version, otherwise return a copy.
__attribute__((warn_unused_result))
char *intern_str(char *str);
// Transfer ownership of a dynamically allocated string to the intern table.
// If an equivalent string was already interned, free `str` and return the existing string.
// Otherwise, store `str` in the table.
__attribute__((warn_unused_result))
char *intern_str_transfer(char *str);
// Free all interned strings and the table used to track them.
void intern_free(void);

// vim: ts=4 sw=0 et cino=L2,l1,(0,W4,m1
