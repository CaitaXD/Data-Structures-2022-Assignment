/**
 * @file defs.h
 * @brief Definitions of the project
 */
#ifndef __TYPEDEFS_H__
#define __TYPEDEFS_H__

// undefine this abomination its defined in windef.h
// not that im including it but the linker is complaining
#undef max 
#undef min 

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "assert.h"

#define CAST(TO, FORM)   ((TO)FORM)

// prints the line number, file name and the function name
#define Trace() printf("Trace: %s:%d:%s \n", __FILE__, __LINE__, __func__);
// readonly pointer to a readonly sequence of chars
typedef char const *const string;
// function that conpares two elements
typedef int (*comparer)(const void *, const void *);
// functions that returns a string representation of an element
typedef char *(*cstringfier)(void *);
// function that the do something and dosent return anything
typedef void (*action)(void *const data, ...);

#define KILO 1000
#define MEGA 1000000
#define GIGA 1000000000
#define TERA 1000000000000

#define KB (1 << 10)
#define MB (1 << 20)
#define GB (1 << 30)

#define byte unsigned char

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"

#undef max
static int max(int a, int b) { return (a * (a >= b)) | (b * (b > a)); }

#define GUARD_AGAINST_NULL(x, MESAGE)                                                                                  \
    if (x == NULL)                                                                                                     \
    {                                                                                                                  \
        printf(MESAGE);                                                                                                \
        exit(EXIT_FAILURE);                                                                                            \
    }

#define FILE_RESET(F) fseek(F, 0, SEEK_SET);

#endif // __TYPEDEFS_H__