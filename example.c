// A simple example program that lets users store and look up hash map values.
// Compile with `make example`
#include <string.h>
#include <stdio.h>

#include "bhash.h"

int main(void)
{
    hashmap_t *h = hashmap_new(NULL);

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
            char *key = strndup(line, (size_t)(eq-line));
            // Use intern_str_transfer() here so we don't have to free(key) afterwards
            key = intern_str_transfer(key);
            // Use intern_str() here since this is part of `line` and we don't want it freed
            char *value = eq[1] ? intern_str(eq+1) : NULL;

            // Store the value in the hash map:
            hashmap_set(h, key, value);
        } else {
            // Intern the key:
            char *key = intern_str(line);

            // Get the value from the hash map:
            char *result = hashmap_get(h, key);
            if (result) printf("%s\n", result);
            else printf("(nil)\n");
        }
    }

    printf("Final values:\n");
    // Iterate over the hash map and print each entry:
    for (char *key = NULL; (key = hashmap_next(h, key)); )
        printf("%s = %s\n", key, hashmap_get(h, key));

    hashmap_free(&h);
    intern_free();
    return 0;
}
