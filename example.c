// A simple example program that lets users store and look up hash map values.
// Compile with `cc *.o example.c -o example` or `cc example.c -o example -lbhash`
#include <string.h>
#include <stdio.h>

#include "bhash.h"

int main(void)
{
    printf("Type either key=value pairs to assign to the hash, or key values to look up.\n");
    hashmap_t *h = new_hashmap(NULL);
    for (;;) { 
        printf("> ");
        char *line = NULL;
        size_t size = 0;
        ssize_t linelen = getline(&line, &size, stdin);
        if (linelen <= 1)
            break;

        line[linelen-1] = '\0'; // strip newline

        char *eq = strchr(line, '=');
        if (eq) { // Assign
            char *key = strndup(line, (size_t)(eq-line));
            key = (char*)str_intern_transfer(key);
            char *value = (char*)str_intern(eq+1);
            hashmap_set(h, key, value);
        } else {
            line = (char*)str_intern(line);
            char *result = hashmap_get(h, line);
            if (result) printf("%s\n", result);
            else printf("(nil)\n");
        }
    }
    free_hashmap(&h);
    free_interned();
    return 0;
}
