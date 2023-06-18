#include "api.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int writeToFile(const char *filename, const char *data) {
    FILE *f = fopen(filename, "wb");
    size_t size = strlen(data);

    if (f == NULL) {
        return 0;
    }

    for (int i = 0; i < size; i++) {
        fputc(data[i], f);
    }

    fclose(f);

    return 1;
}

char *readFromFile(const char *filename) {
    FILE* f = fopen(filename, "rb");
    char *str = malloc(1);
    int len = 1;

    if (f == NULL) {
        return NULL;
    }

    while (!feof(f)) {
        str = realloc(str, len + 1);
        str[len - 1] = (char) fgetc(f);
        len++;
    }
    str[len - 2] = '\0';

    fclose(f);
    return str;
}
