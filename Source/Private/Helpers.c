#include "Helpers.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

Optional_FatString ReadFileToString(char const *filename) {
    FILE *hFile = fopen(filename, "rb");

    if (hFile == nullptr) {
        fprintf(stderr, "Error: file %s was not able to be opened\n", filename);
        return NONE(Optional_FatString);
    }

    // get file size
    fseek(hFile, 0, SEEK_END);
    int64_t len = ftell(hFile);
    fseek(hFile, 0, SEEK_SET);

    if (len < 0) {
        fprintf(stderr, "Error: unexpected error reading file %s \n", filename);
        fclose(hFile);
        return NONE(Optional_FatString);
    }

    size_t size = (size_t)len;

    char *buf = malloc(size);
    if (!buf) {
        fclose(hFile);
        return NONE(Optional_FatString);
    }

    // read file contents
    size_t bytesRead = fread(buf, 1, size, hFile);

    fclose(hFile);

    // if we didn't read the full file, free and return NULL
    if (bytesRead != size) {
        free(buf);
        return NONE(Optional_FatString);
    }

    FatString s = {buf, size};
    return SOME(Optional_FatString, s);
}
