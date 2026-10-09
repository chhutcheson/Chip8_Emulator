#ifndef CHIP8_HELPERS_H
#define CHIP8_HELPERS_H

#include <stddef.h>

// Result sometype
#define OPTIONAL(T) struct { bool Ok; T Value; }
#define DECLARE_OPTIONAL(T, Name) \
    typedef struct { bool Ok; T Value; } Name

#define SOME(Name, v) ((Name){ .Ok = true, .Value = (v) })
#define NONE(Name)    ((Name){ .Ok = false })

typedef struct {
    char    *String;
    size_t  Length;
} FatString;

DECLARE_OPTIONAL(FatString, Optional_FatString);

Optional_FatString ReadFileToString(char const *filename);

#endif // CHIP8_HELPERS_H
