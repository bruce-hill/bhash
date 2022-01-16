#pragma once

const char *str_intern(char *str);
const char *str_intern_transfer(char *str);
void free_interned(void);
