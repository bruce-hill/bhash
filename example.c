// A simple example program that lets users store and look up hash map values.
// Compile with `make example`

#include <gc.h>
#include <intern.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bhash.h"

int main(void)
{
    GC_INIT();
    hashmap_set_allocator(GC_malloc, GC_free);
    hashmap_t *h = hashmap_new();

    printf("Type either key=value pairs to assign to the hash, or key values to look up.\n");
    char *line = NULL;
    size_t size = 0;
    for (;;) { 
        printf("> ");
        ssize_t linelen = getline(&line, &size, stdin);
        if (linelen <= 1)
            break;

        line[linelen-1] = '\0'; // strip newline

        char *eq = strchr(line, '=');
        if (eq) { // Assign
            const char *key = intern_strn(line, (size_t)(eq - line));
            const char *value = eq[1] ? intern_str(eq+1) : NULL;

            // Store the value in the hash map:
            hashmap_set(h, key, value);
        } else {
            // Intern the key:
            const char *key = intern_str(line);

            // Get the value from the hash map:
            const char *result = hashmap_get(h, key);
            if (result) printf("%s\n", result);
            else printf("(nil)\n");
        }
    }
    if (line) free(line);

    printf("Final values:\n");
    // Iterate over the hash map and print each entry:
    for (const char *key = NULL; (key = hashmap_next(h, key)); )
        printf("%s = %s\n", key, hashmap_get(h, key));

    hashmap_free(&h);
    return 0;
}
