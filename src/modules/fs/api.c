#include "api.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int writeToFile(char *filename, unsigned char *data) {
    FILE *f = fopen(filename, "wb");

    if (f == NULL) {
        return 0;
    }

    fwrite(data, sizeof(unsigned char), strlen((char*) data), f);

    fclose(f);

    return 1;
}

unsigned char *readFromFile(char *filename) {
    FILE* f = fopen(filename, "rb");
    unsigned char *str = (unsigned char*) malloc(sizeof(unsigned char));
    int len = 1;
    unsigned char *copy = str;

    if (f == NULL) {
        return NULL;
    }

    while (!feof(f)) {
        *copy = fgetc(f);
        str = (unsigned char*) realloc(str, ++len);
        copy++;
    }
    fclose(f);

    str = (unsigned char*) realloc(str, ++len);
    copy++;
    *copy = 0;

    return str;
}
