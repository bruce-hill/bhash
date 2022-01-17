// A simple example program that lets users store and look up hash map values.
// Compile with `make example`
#include <string.h>
#include <stdio.h>

#include "bhash.h"

int main(void)
{
    hashmap_t *h = new_hashmap(NULL);

    printf("Type either key=value pairs to assign to the hash, or key values to look up.\n");
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
            // Make sure strings are interned before using in the hash table:
            char *key = strndup(line, (size_t)(eq-line));
            key = (char*)str_intern_transfer(key);
            char *value = (char*)str_intern(eq+1);

            // Store the value in the hash map:
            hashmap_set(h, key, value);
        } else {
            // Intern the key:
            line = (char*)str_intern(line);

            // Get the value from the hash map:
            char *result = hashmap_get(h, line);
            if (result) printf("%s\n", result);
            else printf("(nil)\n");
        }
    }

    printf("Final values:\n");
    // Iterate over the hash map and print each entry:
    for (char *key = NULL; (key = hashmap_next(h, key)); )
        printf("%s = %s\n", key, hashmap_get(h, key));

    free_hashmap(&h);
    free_interned();
    return 0;
}
