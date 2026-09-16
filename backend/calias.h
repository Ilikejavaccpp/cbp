// File for Aliases
// for various languages and especially the C ENhanced,
// and/or python
#include "ccompare.h"

/* CEN */
/* linux (arch 7.1.15-arch1-2) */
typedef unsigned char sint;
typedef sint u8; // holy C aliases
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long u64;
#if (defined(__linux__) || defined(__APPLE__)) &&                              \
    (defined(__clang__) || defined(__GNUC__))
typedef __uint128_t u128;
#else
typedef unsigned long long u128;
#endif

#define CEN__STR_MAX_LEN 2147483647
#define CEN__SSTR_MAX_LEN 65535

/* Python */
typedef void None;

#define print_others(objects) printf("%s", objects)

#define print(objects, sep, end, file, flush)                                  \
    _Generic((objects),                                                        \
      str: str_print(&objects, file),                            \
      str*: str_print(objects, file),                          \
      default: print_others(objects)                                                 \
    );                                                                         \
    fprintf(file, "%s", end);                                                  \
    if (flush) fflush(file);                                                   \
