#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <ctime>
#include <string>
#include <vector>
#include <filesystem>


#define MINIMAL
#ifndef MINIMAL
#include "../backend/calias.h"
#include "../backend/cstr.h"
#include "../backend/colors.c"
#include "../backend/cstr.c"
#else

// bootstrap instead of relying on headers
/* calias.h */
#pragma region calias
    #ifndef CEN__ALIAS_H
    #define CEN__ALIAS_H

    // File for Aliases
    // for various languages and especially the C ENhanced,
    // and/or python
    /* ccompare.h */
    #pragma region ccompare
        #ifndef CEN__COMPARE_H
        #define CEN__COMPARE_H

        // The comparison value between two objects A and B
        // such that
        // A >/=/< B
        //
        // Read as "A is `value` than B"
        typedef enum : signed char {
        GREATER = 1,
        EQUAL = 0,
        LESSER = -1,
        } _cmp_val;

        #endif
    #pragma endregion ccompare

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

    typedef signed char ssint;
    typedef signed char i8;
    typedef signed short int i16;
    typedef signed int i32;
    typedef signed long int i64;


    typedef float float32;
    typedef float f32;
    typedef double float64;
    typedef double f64;

    #ifndef CEN__FLAG_OPTIMIZED
    /* don't use this since no SIMD support */
        typedef long double float80;
        typedef long double f80;
    #endif

    #if (defined(__linux__) || defined(__APPLE__)) &&                              \
        (defined(__clang__) || defined(__GNUC__))
        typedef __fp16 float16;
        typedef __fp16 f16;
    #else
        typedef unsigned i16;
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

    #endif
#pragma endregion calias

/* cstr.h */
#pragma region cstr

    #ifndef CEN__STR_H
    #define CEN__STR_H 1

    #define CEN__STR_VERSION 20260805L
    #define CEN__STR_VERSION__UNSTABLE 20260907L

    #ifdef __cplusplus
    extern "C" {
    #endif

    #include <stdio.h>

    #pragma region ccompare
        #ifndef CEN__COMPARE_H
        #define CEN__COMPARE_H

        // The comparison value between two objects A and B
        // such that
        // A >/=/< B
        //
        // Read as "A is `value` than B"
        typedef enum : signed char {
        GREATER = 1,
        EQUAL = 0,
        LESSER = -1,
        } _cmp_val;

        #endif
    #pragma endregion ccompare
    #pragma region calias
        #ifndef CEN__ALIAS_H
        #define CEN__ALIAS_H

        // File for Aliases
        // for various languages and especially the C ENhanced,
        // and/or python
        /* ccompare.h */
        #pragma region ccompare
            #ifndef CEN__COMPARE_H
            #define CEN__COMPARE_H

            // The comparison value between two objects A and B
            // such that
            // A >/=/< B
            //
            // Read as "A is `value` than B"
            typedef enum : signed char {
            GREATER = 1,
            EQUAL = 0,
            LESSER = -1,
            } _cmp_val;

            #endif
        #pragma endregion ccompare

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

        typedef signed char ssint;
        typedef signed char i8;
        typedef signed short int i16;
        typedef signed int i32;
        typedef signed long int i64;


        typedef float float32;
        typedef float f32;
        typedef double float64;
        typedef double f64;

        #ifndef CEN__FLAG_OPTIMIZED
        /* don't use this since no SIMD support */
            typedef long double float80;
            typedef long double f80;
        #endif

        #if (defined(__linux__) || defined(__APPLE__)) &&                              \
            (defined(__clang__) || defined(__GNUC__))
            typedef __fp16 float16;
            typedef __fp16 f16;
        #else
            typedef unsigned i16;
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

        #endif
    #pragma endregion calias

    /* you may override this to change the arena size */
    #define CEN__STR_ARENA_SIZE 4096

    /* the string structure, with a terminating and
    * stack-allocated buffer for ok ish strings. good
    * for your stupid essays or long paras in 3 different
    * files
    */
    typedef struct {
    char *pointer;
    int length; /* overkill, it is over 2 billion */
    } str;

    /* the string structure for small strings,
    * with a stack-allocated buffer for small strings.
    * good for helloworld, menu helps, paras, basically,
    * the memory efficent gold standard
    */
    typedef struct {
    char pointer[CEN__SSTR_MAX_LEN];
    u16 length; /* overkill, it is over 2 billion */
    } sstr;

    /* the string structure for tiny strings,
    * with a stack-allocated buffer for tiny strings.
    * good for menus, buttons, prompts, files (names)
    */
    typedef struct {
    char *pointer;
    u8 length; /* underfitting, it is literaly smaller than stuff */
    } tstr;

    /* simple arena for strings */
    typedef struct {
    char buffer[CEN__STR_ARENA_SIZE];
    int offset;
    int capacity;
    } str_t;

    // here you can pass a C literal and/or a const char * variable
    str str_init(const char *c_literal);
    void str_destroy(str *self);

    void str_initArena(str_t *arena, int *offset);

    str *str_append(str *self, const char *c_string, str_t *arena);
    str *str_append_heap(str *self, const char *c_string);
    str *str_delete(str *self, int index, int number);
    str *str_change(str *self, const char *c_string, int index, str_t *arena);
    str str_mutate(str *self, const char *c_string, int index, str_t *arena); /* non chainable */

    // same return type (enum member) as stdc library functions
    _cmp_val str_cmp(str *self, str *other);

    // for enabling others
    bool str_isequalto(str *self, const char *other);
    bool str_isempty(str *self);

    // showing (prettier)
    void str_print(str *self, FILE *out);
    void str_printf(str *self, FILE *out, ...);

    #ifdef __cplusplus
    }
    #endif

    #endif
#pragma endregion cstr

/* colors.c */
#pragma region colors
    #ifdef __cplusplus
    extern "C" {
    #endif

    #ifndef CEN__COLORS_C_H
    #define CEN__COLORS_C_H

    #include <stdbool.h>
    // #define CEN__STR_ARENA_SIZE 32

    #pragma region calias
        #ifndef CEN__ALIAS_H
        #define CEN__ALIAS_H

        // File for Aliases
        // for various languages and especially the C ENhanced,
        // and/or python
        /* ccompare.h */
        #pragma region ccompare
            #ifndef CEN__COMPARE_H
            #define CEN__COMPARE_H

            // The comparison value between two objects A and B
            // such that
            // A >/=/< B
            //
            // Read as "A is `value` than B"
            typedef enum : signed char {
            GREATER = 1,
            EQUAL = 0,
            LESSER = -1,
            } _cmp_val;

            #endif
        #pragma endregion ccompare

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

        typedef signed char ssint;
        typedef signed char i8;
        typedef signed short int i16;
        typedef signed int i32;
        typedef signed long int i64;


        typedef float float32;
        typedef float f32;
        typedef double float64;
        typedef double f64;

        #ifndef CEN__FLAG_OPTIMIZED
        /* don't use this since no SIMD support */
            typedef long double float80;
            typedef long double f80;
        #endif

        #if (defined(__linux__) || defined(__APPLE__)) &&                              \
            (defined(__clang__) || defined(__GNUC__))
            typedef __fp16 float16;
            typedef __fp16 f16;
        #else
            typedef unsigned i16;
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

        #endif
    #pragma endregion calias

    #pragma region cstr

        #ifndef CEN__STR_H
        #define CEN__STR_H 1

        #define CEN__STR_VERSION 20260805L
        #define CEN__STR_VERSION__UNSTABLE 20260907L

        #ifdef __cplusplus
        extern "C" {
        #endif

        #include <stdio.h>

        #pragma region ccompare
            #ifndef CEN__COMPARE_H
            #define CEN__COMPARE_H

            // The comparison value between two objects A and B
            // such that
            // A >/=/< B
            //
            // Read as "A is `value` than B"
            typedef enum : signed char {
            GREATER = 1,
            EQUAL = 0,
            LESSER = -1,
            } _cmp_val;

            #endif
        #pragma endregion ccompare
        #pragma region calias
            #ifndef CEN__ALIAS_H
            #define CEN__ALIAS_H

            // File for Aliases
            // for various languages and especially the C ENhanced,
            // and/or python
            /* ccompare.h */
            #pragma region ccompare
                #ifndef CEN__COMPARE_H
                #define CEN__COMPARE_H

                // The comparison value between two objects A and B
                // such that
                // A >/=/< B
                //
                // Read as "A is `value` than B"
                typedef enum : signed char {
                GREATER = 1,
                EQUAL = 0,
                LESSER = -1,
                } _cmp_val;

                #endif
            #pragma endregion ccompare

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

            typedef signed char ssint;
            typedef signed char i8;
            typedef signed short int i16;
            typedef signed int i32;
            typedef signed long int i64;


            typedef float float32;
            typedef float f32;
            typedef double float64;
            typedef double f64;

            #ifndef CEN__FLAG_OPTIMIZED
            /* don't use this since no SIMD support */
                typedef long double float80;
                typedef long double f80;
            #endif

            #if (defined(__linux__) || defined(__APPLE__)) &&                              \
                (defined(__clang__) || defined(__GNUC__))
                typedef __fp16 float16;
                typedef __fp16 f16;
            #else
                typedef unsigned i16;
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

            #endif
        #pragma endregion calias

        /* you may override this to change the arena size */
        #define CEN__STR_ARENA_SIZE 4096

        /* the string structure, with a terminating and
        * stack-allocated buffer for ok ish strings. good
        * for your stupid essays or long paras in 3 different
        * files
        */
        typedef struct {
        char *pointer;
        int length; /* overkill, it is over 2 billion */
        } str;

        /* the string structure for small strings,
        * with a stack-allocated buffer for small strings.
        * good for helloworld, menu helps, paras, basically,
        * the memory efficent gold standard
        */
        typedef struct {
        char pointer[CEN__SSTR_MAX_LEN];
        u16 length; /* overkill, it is over 2 billion */
        } sstr;

        /* the string structure for tiny strings,
        * with a stack-allocated buffer for tiny strings.
        * good for menus, buttons, prompts, files (names)
        */
        typedef struct {
        char *pointer;
        u8 length; /* underfitting, it is literaly smaller than stuff */
        } tstr;

        /* simple arena for strings */
        typedef struct {
        char buffer[CEN__STR_ARENA_SIZE];
        int offset;
        int capacity;
        } str_t;

        // here you can pass a C literal and/or a const char * variable
        str str_init(const char *c_literal);
        void str_destroy(str *self);

        void str_initArena(str_t *arena, int *offset);

        str *str_append(str *self, const char *c_string, str_t *arena);
        str *str_append_heap(str *self, const char *c_string);
        str *str_delete(str *self, int index, int number);
        str *str_change(str *self, const char *c_string, int index, str_t *arena);
        str str_mutate(str *self, const char *c_string, int index, str_t *arena); /* non chainable */

        // same return type (enum member) as stdc library functions
        _cmp_val str_cmp(str *self, str *other);

        // for enabling others
        bool str_isequalto(str *self, const char *other);
        bool str_isempty(str *self);

        // showing (prettier)
        void str_print(str *self, FILE *out);
        void str_printf(str *self, FILE *out, ...);

        #ifdef __cplusplus
        }
        #endif

        #endif
    #pragma endregion cstr

    #ifndef MINIMAL
    /* some pre-defined colors (CRUCIAL) */
    #define COLOR_RESET                                                            \
    "\033[0m" /* reset color, italics, bold, dim (basicaly Alpha) */
    #define COLOR_RESET_BOLD "\033[22m"   /* reset bolds */
    #define COLOR_RESET_ITALIC "\033[23m" /* reset italics */

    #define COLOR_DIM "\033[2m"
    #define COLOR_ITALIC "\033[3m"
    #define COLOR_BOLD "\033[1m"

    // red
    #define COLOR_RED "\033[31m"        /* standard  (fg) */
    #define COLOR_RED_BR "\033[0;91m"   /* bright    (fg)*/
    #define COLOR_RED_BO "\033[1;31m"   /* bold      (fg)*/
    #define COLOR_RED_BG "\033[41m"     /* standard (bg) */
    #define COLOR_RED_BG_BR "\033[101m" /* bright (bg) */

    // green
    #define COLOR_GREEN "\033[32m"
    #define COLOR_GREEN_BR "\033[0;92m"
    #define COLOR_GREEN_BO "\033[1;32m"
    #define COLOR_GREEN_BG "\033[42m"
    #define COLOR_GREEN_BG_BR "\033[102m"

    // yellow
    #define COLOR_YEL "\033[33m"
    #define COLOR_YEL_BR "\033[0;93m"
    #define COLOR_YEL_BO "\033[1;33m"
    #define COLOR_YEL_BG "\033[43m"
    #define COLOR_YEL_BG_BR "\033[103m"

    // blue
    #define COLOR_BLUE "\033[34m"
    #define COLOR_BLUE_BR "\033[0;94m"
    #define COLOR_BLUE_BO "\033[1;34m"
    #define COLOR_BLUE_BG "\033[44m"
    #define COLOR_BLUE_BG_BR "\033[104m"

    // magent
    #define COLOR_MAG "\033[35m"
    #define COLOR_MAG_BR "\033[0;95m"
    #define COLOR_MAG_BO "\033[1;35m"
    #define COLOR_MAG_BG "\033[45m"
    #define COLOR_MAG_BG_BR "\033[105m"

    // cyan
    #define COLOR_CYAN "\033[36m"
    #define COLOR_CYAN_BR "\033[0;96m"
    #define COLOR_CYAN_BO "\033[1;36m"
    #define COLOR_CYAN_BG "\033[46m"
    #define COLOR_CYAN_BG_BR "\033[106m"

    // white
    #define COLOR_WHITE "\033[37m"
    #define COLOR_WHITE_BR "\033[0;97m"
    #define COLOR_WHITE_BO "\033[1;37m"
    #define COLOR_WHITE_BG "\033[47m"
    #define COLOR_WHITE_BG_BR "\033[107m"

    // black / grey
    #define COLOR_BLACK "\033[30m"
    #define COLOR_BLACK_BR "\033[0;90m" // Dark Grey / Grey
    #define COLOR_BLACK_BO "\033[1;30m"
    #define COLOR_BLACK_BG "\033[40m"
    #define COLOR_BLACK_BG_BR "\033[100m"

    // aliases
    #define COLOR_GRAY "\033[0;90m" // Grey / Dark Grey
    #define COLOR_GRAY_DR                                                         \
    "\033[0;90m" // DR for Dark, Dark Grey (Terminal Emulator dependant!)
    #define COLOR_GRAY_BG "\033[100m"
    #endif

    // static in the array brackets ensures that there are
    // minimum 7 characters (C99+ only, omitted in C++)
    #ifdef __cplusplus
    str hex_to_ansi(const char *hex, str_t *arena, bool bg) {
    #else
    str hex_to_ansi(const char hex[static 7], str_t *arena, bool bg) {
    #endif
        if (hex == NULL || hex[0] != '#' || arena == NULL)
            return (str){NULL, 0};

        unsigned int r = 0, g = 0, b = 0;
        if (sscanf(hex, "#%2x%2x%2x", &r, &g, &b) != 3)
            return (str){NULL, 0};

        // ANSI 24-bit truecolor foreground format: \033[38;2;R;G;Bm
        if (arena->offset + 24 > arena->capacity)
            return (str){NULL, 0};

        char *ret_p = arena->buffer + arena->offset;
        u8 prefix = (bg) ? 48 : 38;

        int written = snprintf(ret_p, 24, "\033[%d;2;%u;%u;%um", prefix, r, g, b);

        if (written < 0)
            return (str){NULL, 0};

        arena->offset += written + 1;

        return (str){
            .pointer = ret_p,
            .length = written,
        };
    }

    #endif

    #ifdef __cplusplus
    }
    #endif
#pragma endregion colors

/* cstr.c */
#pragma region cstrc

    #ifdef __cplusplus
    extern "C" {
    #endif

    #ifndef CEN__STR_C_H
    #define CEN__STR_C_H


    #include <malloc.h>
    #include <stdarg.h>
    #include <stdio.h>
    #include <string.h>

    #pragma region calias
        #ifndef CEN__ALIAS_H
        #define CEN__ALIAS_H

        // File for Aliases
        // for various languages and especially the C ENhanced,
        // and/or python
        /* ccompare.h */
        #pragma region ccompare
            #ifndef CEN__COMPARE_H
            #define CEN__COMPARE_H

            // The comparison value between two objects A and B
            // such that
            // A >/=/< B
            //
            // Read as "A is `value` than B"
            typedef enum : signed char {
            GREATER = 1,
            EQUAL = 0,
            LESSER = -1,
            } _cmp_val;

            #endif
        #pragma endregion ccompare

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

        typedef signed char ssint;
        typedef signed char i8;
        typedef signed short int i16;
        typedef signed int i32;
        typedef signed long int i64;


        typedef float float32;
        typedef float f32;
        typedef double float64;
        typedef double f64;

        #ifndef CEN__FLAG_OPTIMIZED
        /* don't use this since no SIMD support */
            typedef long double float80;
            typedef long double f80;
        #endif

        #if (defined(__linux__) || defined(__APPLE__)) &&                              \
            (defined(__clang__) || defined(__GNUC__))
            typedef __fp16 float16;
            typedef __fp16 f16;
        #else
            typedef unsigned i16;
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

        #endif
    #pragma endregion calias

    #pragma region cstr

        #ifndef CEN__STR_H
        #define CEN__STR_H 1

        #define CEN__STR_VERSION 20260805L
        #define CEN__STR_VERSION__UNSTABLE 20260907L

        #ifdef __cplusplus
        extern "C" {
        #endif

        #include <stdio.h>

        #pragma region ccompare
            #ifndef CEN__COMPARE_H
            #define CEN__COMPARE_H

            // The comparison value between two objects A and B
            // such that
            // A >/=/< B
            //
            // Read as "A is `value` than B"
            typedef enum : signed char {
            GREATER = 1,
            EQUAL = 0,
            LESSER = -1,
            } _cmp_val;

            #endif
        #pragma endregion ccompare
        #pragma region calias
            #ifndef CEN__ALIAS_H
            #define CEN__ALIAS_H

            // File for Aliases
            // for various languages and especially the C ENhanced,
            // and/or python
            /* ccompare.h */
            #pragma region ccompare
                #ifndef CEN__COMPARE_H
                #define CEN__COMPARE_H

                // The comparison value between two objects A and B
                // such that
                // A >/=/< B
                //
                // Read as "A is `value` than B"
                typedef enum : signed char {
                GREATER = 1,
                EQUAL = 0,
                LESSER = -1,
                } _cmp_val;

                #endif
            #pragma endregion ccompare

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

            typedef signed char ssint;
            typedef signed char i8;
            typedef signed short int i16;
            typedef signed int i32;
            typedef signed long int i64;


            typedef float float32;
            typedef float f32;
            typedef double float64;
            typedef double f64;

            #ifndef CEN__FLAG_OPTIMIZED
            /* don't use this since no SIMD support */
                typedef long double float80;
                typedef long double f80;
            #endif

            #if (defined(__linux__) || defined(__APPLE__)) &&                              \
                (defined(__clang__) || defined(__GNUC__))
                typedef __fp16 float16;
                typedef __fp16 f16;
            #else
                typedef unsigned i16;
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

            #endif
        #pragma endregion calias

        /* you may override this to change the arena size */
        #define CEN__STR_ARENA_SIZE 4096

        /* the string structure, with a terminating and
        * stack-allocated buffer for ok ish strings. good
        * for your stupid essays or long paras in 3 different
        * files
        */
        typedef struct {
        char *pointer;
        int length; /* overkill, it is over 2 billion */
        } str;

        /* the string structure for small strings,
        * with a stack-allocated buffer for small strings.
        * good for helloworld, menu helps, paras, basically,
        * the memory efficent gold standard
        */
        typedef struct {
        char pointer[CEN__SSTR_MAX_LEN];
        u16 length; /* overkill, it is over 2 billion */
        } sstr;

        /* the string structure for tiny strings,
        * with a stack-allocated buffer for tiny strings.
        * good for menus, buttons, prompts, files (names)
        */
        typedef struct {
        char *pointer;
        u8 length; /* underfitting, it is literaly smaller than stuff */
        } tstr;

        /* simple arena for strings */
        typedef struct {
        char buffer[CEN__STR_ARENA_SIZE];
        int offset;
        int capacity;
        } str_t;

        // here you can pass a C literal and/or a const char * variable
        str str_init(const char *c_literal);
        void str_destroy(str *self);

        void str_initArena(str_t *arena, int *offset);

        str *str_append(str *self, const char *c_string, str_t *arena);
        str *str_append_heap(str *self, const char *c_string);
        str *str_delete(str *self, int index, int number);
        str *str_change(str *self, const char *c_string, int index, str_t *arena);
        str str_mutate(str *self, const char *c_string, int index, str_t *arena); /* non chainable */

        // same return type (enum member) as stdc library functions
        _cmp_val str_cmp(str *self, str *other);

        // for enabling others
        bool str_isequalto(str *self, const char *other);
        bool str_isempty(str *self);

        // showing (prettier)
        void str_print(str *self, FILE *out);
        void str_printf(str *self, FILE *out, ...);

        #ifdef __cplusplus
        }
        #endif

        #endif
    #pragma endregion cstr

    str str_init(const char *c_literal) {
        if (c_literal == NULL)
            return (str){NULL, 0};
        if (c_literal[0] == 0)
            return (str){(char *)c_literal, 0};

        int i = 1;
        while (1) {
            if (c_literal[i - 1] != 0)
                ++i;
            else
                break;
        }

        return (str){
            .pointer = (char *)c_literal,
            .length = i - 1,
        };
    }

    void  str_initArena(str_t *arena, int *offset) {
        int _offset = 0;
        if (offset == NULL || *offset < 0)
            _offset = 0;
        else
            _offset = *offset;

        *arena = (str_t){
            .buffer = {0},
            .offset = _offset,
            .capacity = CEN__STR_ARENA_SIZE,
        };
    }

    str *str_append(str *self, const char *c_string, str_t *arena) {
        int len = strlen(c_string); /* handy dandy */

        if (len == 0)
            return self;
        if (len == CEN__STR_MAX_LEN)
            return self; /* prevent overflow */
        if (self->length + len > CEN__STR_MAX_LEN)
            return self; /* prevent overflow */

        int total = self->length + len;

        /* Stack-operatable append using a fixed stack buffer of capacity
            * CEN__STR_MAX_LEN (or CEN__SSTR_MAX_LEN). Since capacity is
            * CEN__STR_MAX_LEN, we declare a local stack array and copy data into it, but
            * to return it safely without going out of scope, we either need the caller's
            * struct to embed the array (like sstr) or we use static/heap. However, if
            * `self->pointer` points to caller-provided stack storage, we can append
            * directly: therefore, to keep it nice and simple, we use an ARENA (hehe)
            */

        #ifdef CEN__FLAG_OPTIMIZED
        if (total > CEN__STR_ARENA_SIZE) /* since it is handled  by setter */
            return self; /* prevent overflow */
        #else
        if (arena->offset + total + 1 > arena->capacity) return self;
        #endif

        char * dest = arena->buffer + arena->offset;

        memcpy(dest, self->pointer, self->length); /* the string */
        memcpy(dest + self->length, c_string, len); /* the new data */
        dest[total] = '\0'; /* terminate the string in arena */

        arena->offset += total + 1;
        self->pointer = dest; /* now they share the same memory in the stack */
        self->length = total;

        return self;
    }

    /* old heap-allocated version of str_append */
    str *str_append_heap(str *self, const char *c_string) {
        int len = strlen(c_string);

        if (len == 0)
            return self;
        if (len == CEN__STR_MAX_LEN)
            return self;

        if (self->length + len > CEN__STR_MAX_LEN)
            return self;

        char *new_pointer = (char *)realloc(self->pointer, self->length + len + 1);
        memcpy(new_pointer + self->length, c_string, len);
        new_pointer[self->length + len] = 0;
        self->pointer = new_pointer;
        self->length += len;

        return self;
    }

    /* number here is the count after index */
    str *str_delete(str *self, int index, int number) {
        /* we want negative / backwards indexing */
        if (index > self->length)
            return self;
        if (index + number > self->length)
            return self;

        // TODO: implement
        // NOTE: this is eh
        if (self == NULL)
            return self;

        /* basically, over here,
        * if number == 1, delete the character at index
        * otherwise, delete the character at index and the next number - 1 characters
        */
        if (number == 1) {
            memmove(&self->pointer[index], &self->pointer[index + 1], self->length - index);
            self->length -= 1;
        } else {
            memmove(&self->pointer[index], &self->pointer[index + number], self->length - (index + number - 1));
            self->length -= number;
        }

        return self;
    }

    /* comparisons */
    _cmp_val str_cmp(str *self, str *other) {
        if (self->length != other->length)
            return self->length < other->length ? LESSER : GREATER; /* -1 or 1 */
        for (int i = 0; i < self->length; i++) {
            if (self->pointer[i] != other->pointer[i])
                return self->pointer[i] < other->pointer[i] ? LESSER : GREATER; /* -1 or 1 */
        }
        return EQUAL; /* 0 */
    }
    bool str_isequalto(str *self, const char *other) {
        return self->pointer == other && self->length == strlen(other);
    }
    bool str_isempty(str *self) {
        return self->length == 0 && self->pointer[0] == 0;
    }

    /* printing / formatting */
    void str_print(str *self, FILE *out) {
        if (out == NULL)
            out = stderr; /* instant, no buffered */
        fprintf(out, "CEN String: %s\n", self->pointer);
    }
    void str_printf(str *self, FILE *out, ...) {
        if (out == NULL)
            out = stdout;
        va_list args;
        va_start(args, out);
        vfprintf(out, self->pointer, args); /* no modifying, just formatted */
        va_end(args);
    }

    /* experimentals */
    str str_concat(str a, str b) {
        char *result = (char  *)malloc(a.length + b.length + 1);
        memcpy(result, a.pointer, a.length);
        memcpy(result + a.length, b.pointer, b.length);
        result[a.length + b.length] = 0;
        return (str){
            .pointer = result,
            .length = a.length + b.length,
        };
    }

    #endif

    #ifdef __cplusplus
    }
    #endif
#pragma endregion cstrc

#endif

// colors, you may/maynot modify this
#define COLOR_INFO    "#9c1bef" /* pacman / zig info purple */
#define COLOR_SUCCESS "#2ecc71" /* emerald green for success/done */
#define COLOR_WARNING "#f1c40f" /* amber yellow for warnings */
#define COLOR_ERROR   "#e74c3c" /* vibrant red for errors */
#define COLOR_MUTED   "#7f8c8d" /* cool grey for metadata/stats */
#define RESET         "\033[0m" /* reset color */

// necessary non mangled stuff, you aren't allowed to modify
// the art of be safe: encapsulation -- OOP
int _code = 0;
u8 _offset = 0;
str_t _arn;
extern "C" { void build_main(int argc, char *argv[]); void conf_build_cmd(int argc, char *argv[]) { std::cout << hex_to_ansi(COLOR_INFO, &_arn, false).pointer << ":: " << RESET << "Building with command\n" << hex_to_ansi(COLOR_MUTED, &_arn, false).pointer; for (int i = 0; i < argc; ++i) { std::cout << argv[i] << " "; } std::cout << RESET << std::endl;   }; void conf_build_end() { std::cout << hex_to_ansi(COLOR_INFO, &_arn, false).pointer << ":: " << (_code == 0 ? RESET : hex_to_ansi(COLOR_ERROR, &_arn,false).pointer) << "Build finished" << RESET << std::endl; /* immediately flush */ } }
namespace  CBuildP {
    struct opt_t {
        const char  * compiler="clang";
        int level = 0; /* no optimization, like in Zed */
        bool debug = true; /* add debug symbols */

        static void prints(const opt_t &opt);
    };
    void opt_t::prints(const opt_t &opt) {
        std::cout << "   - opt level: " << opt.level << "\n";
        std::cout << "   - debug: " << opt.debug << std::endl;
    }
    opt_t _stdopt = {};
}
namespace CBuildP {
    using file_t = std::string;
    namespace libtype {
        /* encrypt using jumbled keyboard mess */
        const char* STATIC = "ASFUOASOF@#&HDlib.a#QBDKASB";
        const char* SHARED = "NAIUSDFNUA!#*&^!#lib.soH&*#FHB";
    }

    file_t _link = "";
    file_t _flags = "";

    inline std::string execcmd(const std::string &cmd) {
        std::string result = "";
        FILE *pipe = popen(cmd.c_str(), "r");
        if (!pipe) return result;
        char buffer[128];
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            result += buffer;
        }
        pclose(pipe);
        if (!result.empty() && result.back() == '\n') result.pop_back();
        return result;
    }

    bool is_file_C_CXX(const std::string &filename) {
        return  filename.find(".c") != std::string::npos ||
                filename.find(".cxx") != std::string::npos ||
                filename.find(".cpp") != std::string::npos;
    }

    void check_file(const std::string &filename) {
        std::cout << "[ERROR] : Non C/C++ file included, are you sure?\n"
                << "        Included src file is non C/C++: " << filename <<
                std::endl;
        _code = 1;
    }

    std::string get_files(int argc, char *argv[]) {
        if (argc < 2) {
            return "";
        }

        if (!is_file_C_CXX(std::string(argv[1 + _offset]))) {
            std::cout << "[ERROR] : Non C/C++ file included, are you sure?\n"
                    << "        First file/arg is non C/C++: " << argv[0] << " " << argv[1] <<
                std::endl;
            _code = 1;
            return "";
        }

        std::string result = std::string(argv[1 + _offset]);
        for (int i = 2 + _offset; i < argc; ++i) {
            if (is_file_C_CXX(std::string(argv[i])))
                result.append(" " + std::string(argv[i]));
            else {
                std::cout << "[ERROR] : Non C/C++ file included, are you sure?" << std::endl;
                _code = 1;
                return "";
            }
        }
        return result;
    }

    void specs(const file_t &file_names, const file_t &out_file) {
        std::cout << hex_to_ansi(COLOR_INFO, &_arn, false).pointer << ":: " << RESET << "Build info:\n";
        std::cout << hex_to_ansi(COLOR_MUTED, &_arn, false).pointer << "   - C/C++ files : " << RESET << file_names << "\n";
        std::cout << hex_to_ansi(COLOR_MUTED, &_arn, false).pointer << "   - output file : " << RESET << out_file << std::endl;
        std::cout << hex_to_ansi(COLOR_MUTED, &_arn, false).pointer << "   - optimization: " << RESET << _stdopt.level << std::endl;
        std::cout << hex_to_ansi(COLOR_MUTED, &_arn, false).pointer << "   - debug       : " << RESET << _stdopt.debug << std::endl;
        std::cout << hex_to_ansi(COLOR_MUTED, &_arn, false).pointer << "   - compiler    : " << RESET << _stdopt.compiler << std::endl;
    }

    void optimize(const opt_t &opt) {
        std::cout << hex_to_ansi(COLOR_INFO, &_arn, false).pointer << ":: " << RESET << "Optimization stats... \n";
        opt.prints(opt);
        std::cout << hex_to_ansi(COLOR_INFO, &_arn, false).pointer << ":: " << RESET << "Optimizing the standard";

        const char *loading_state[] = {
            // "⢿", "⣻", "⣽"
            ".  ", ".. ", "..."
        };

        for (int i = 0; i < 12; ++i) {
            std::cout << "\r" << hex_to_ansi(COLOR_INFO, &_arn, false).pointer << ":: " << RESET << "Optimizing the standard " << loading_state[i % 3] << "\033[K" << std::flush;
            clock_t start_time = clock();
            while (clock() < start_time + (CLOCKS_PER_SEC / 4));
        }

        _stdopt = opt;

        std::cout << "\n" << hex_to_ansi(COLOR_SUCCESS, &_arn, false).pointer << ":: " << RESET << hex_to_ansi(COLOR_SUCCESS, &_arn, false).pointer << "DONE" << std::endl;
    }

    void link(const std::vector<file_t>& libraries, const std::vector<file_t> &look_for_dirs) {
        file_t _stor = "";
        file_t _dirl = "";

        if (libraries.empty()) return;

        // look for the libraries in the dirs
        for (int i = 0; i < look_for_dirs.size(); ++i) {
            _dirl += "-L" + look_for_dirs[i] + " ";
            // if (i < look_for_dirs.size() - 1) _dirl += " ";
        }

        if (!look_for_dirs.empty())
            _link += _dirl;

        // iterate over the libraries and append them.
        for (int i = 0; i < libraries.size(); ++i) {
            _link += "-l" + libraries[i] + " ";
            _stor += libraries[i] + (i < libraries.size() - 1 ? " " : "");
        }

        std::cout << hex_to_ansi(COLOR_INFO, &_arn, false).pointer << ":: " << RESET << "Linking libraries: " << hex_to_ansi(COLOR_MUTED, &_arn, false).pointer << _stor << RESET << std::endl;
    }

    void include(const std::vector<file_t> &dirs) {
        for (size_t i = 0; i < dirs.size(); ++i)
            _flags += "-I" + dirs[i] + " ";
    }

    // compile the C/CXX flags like `-pthread`, `-Iinclude`, etc.
    // this is dangerous...
    void compile_flags(const std::vector<file_t> &flags) {
        for (size_t i = 0; i < flags.size(); ++i) {
            if (!flags[i].empty() && (flags[i][1] == 'p' || flags[i][1] == 'I'))
                _flags += flags[i] + " ";
        }
    }

    void compile_link(const std::vector<std::string> &link_cmd_comp) {
        for (size_t i = 0; i < link_cmd_comp.size(); ++i) {
            _link += link_cmd_comp[i] + " ";
        }
    }

    void compile_lib(const std::vector<file_t> &file_names, const file_t &libname,
                    const file_t &dir = "cbp.build/lib",
                    const std::vector<file_t> &libtypes = {},
                    const bool &clean = true)
    {
        if (file_names.empty() || libname.empty()) return;

        /* safety: ensure that the directory exists */
        std::string __ensure_dir__mkdir_c = "mkdir -p " + dir;
        system(__ensure_dir__mkdir_c.c_str());

        /* safety: make sure that the object directory exists */
        CBuildP::file_t obj_dir = dir + "/compiled";
        std::string __ensure_obj_dir__mkdir_c = "mkdir -p " + obj_dir;
        system(__ensure_obj_dir__mkdir_c.c_str());

        /* stats */
        std::cout << hex_to_ansi(COLOR_INFO, &_arn, false).pointer << ":: " << RESET << "Compiling library: " << hex_to_ansi(COLOR_MUTED, &_arn, false).pointer;
        for (size_t i = 0; i < file_names.size(); ++i)
            std::cout << file_names[i] << (i + 1 < file_names.size() ? ", " : "");
        std::cout << RESET << std::endl;

        /* first compile the object files */
        std::cout << hex_to_ansi(COLOR_INFO, &_arn, false).pointer << ":: " << RESET << "Compiling object files" << RESET << std::endl;

        /* compile the object files */
        bool _succ = true;
        std::vector<std::string> obj_files;
        for (size_t i = 0; i < file_names.size(); ++i) {
            std::string mangle_name = file_names[i].substr(0, file_names[i].find_last_of('.'));

            for (u16 j = 0; j < mangle_name.length(); ++j)
                if (mangle_name[j] == '/') {
                    mangle_name[j] = '_';
                }

            CBuildP::file_t obj = obj_dir + "/Zd_." + mangle_name + ".o";
            std::string command = _stdopt.compiler + std::string(" -O") + std::to_string(_stdopt.level) + std::string(" -c ") +
                                file_names[i] + " -o " +
                                // dir + "/" + file_names[i].substr(0, file_names[i].find_last_of('.')) + ".o";
                                obj;

            int result = system(command.c_str());
            if (result != 0) {
                std::cout << hex_to_ansi(COLOR_ERROR, &_arn, false).pointer << ":: " << RESET << "[ERROR] : Library Compilation " << RESET << "failed" << std::endl;
                _code = 1;
                _succ = false;
            } else {
                obj_files.push_back(obj);
                std::cout << hex_to_ansi(COLOR_SUCCESS, &_arn, false).pointer << ":: " << RESET << "Library Compilation " << RESET << "succeeded in compiling " << file_names[i] << " into " << obj << std::endl;
            }
        }

        /* if compilation failed, exit early */
        if (!_succ) return; // this is a bit of bloat since you may just slap that at the
                            // result check in the for loop iteration

        /* link the object files */
        std::string ar_command = "ar rcs " + dir + "/lib" + libname + ".a";
        // for (size_t i = 0; i < file_names.size(); ++i) {
        //     ar_command += " " + dir + "/" + file_names[i].substr(0, file_names[i].find_last_of('.')) + ".o";
        // }
        for (const file_t & o : obj_files) {
            ar_command += " " + o;
        }
        int result = system(ar_command.c_str());
        if (result != 0) {
            std::cout << hex_to_ansi(COLOR_ERROR, &_arn, false).pointer << ":: " << RESET << "[ERROR] : Library Linking " << RESET << "failed" << std::endl;
            _code = 1;
        } else {
            std::cout << hex_to_ansi(COLOR_SUCCESS, &_arn, false).pointer << ":: " << RESET << "Library Linking " << RESET << "succeeded" << std::endl;
            std::cout << hex_to_ansi(COLOR_INFO, &_arn, false).pointer << ":: " << RESET << "Library " << hex_to_ansi(COLOR_MUTED, &_arn, false).pointer << "lib" << libname << ".a" << RESET << " created" << std::endl;
            std::cout << hex_to_ansi(COLOR_INFO, &_arn, false).pointer << ":: " << RESET << hex_to_ansi(COLOR_SUCCESS, &_arn, false).pointer << "DONE" << RESET << std::endl;
        }

        /* clean: optional */
        if (clean) {
            for (const file_t &obj_file : obj_files) {
                // std::string obj_file = dir + "/" + file_names[i].substr(0, file_names[i].find_last_of('.')) + ".o"; // get the object file path
                std::string rm_cmd = "rm -f " + obj_file; // command
                system(rm_cmd.c_str()); // new shell with the rm command executed.
            }
            if (std::filesystem::is_empty(obj_dir)) {
                std::filesystem::remove(obj_dir);
            }
        }
    }

    void compile(const file_t &file_names, const file_t & outfile) {
        std::cout << hex_to_ansi(COLOR_INFO, &_arn, false).pointer << ":: " << RESET << "Compiling (with linkage): " << hex_to_ansi(COLOR_MUTED, &_arn, false).pointer << file_names << RESET << std::endl;
        std::string command = std::string(_stdopt.compiler) + " " + _flags + " " + file_names + " " + _link +  " -o " + outfile;
        int result = system(command.c_str());
        if (result != 0) {
            std::cout << hex_to_ansi(COLOR_ERROR, &_arn, false).pointer << ":: " << RESET << "[ERROR] : Compilation " << RESET << "failed" << std::endl;
            _code = 1;
        } else {
            std::cout << "\n" << hex_to_ansi(COLOR_INFO, &_arn, false).pointer << ":: " << RESET << hex_to_ansi(COLOR_SUCCESS, &_arn, false).pointer << "Compilation successful\n";
        }
    }

    namespace compilers {
        namespace c {
            const char* clang = "clang";
            const char* gcc = "gcc";
        }
        namespace cpp {
            const char* clang = "clang++";
            const char* gcc = "g++";
        }
        namespace cxx {
            const char* clang = "clang++";
            const char* gcc = "g++";
        }
        std::vector<const char*> others = {};
    }

    namespace optimization {
        u8 none = 0;
        u8 level1 = 1;
        u8 level2 = 2;
        u8 level3 = 3;

        /* nice aliases */
        u8 max = 3;
        u8 fast = 1;
    }

}
int main(int argc, char *argv[]) { str_initArena(&_arn, NULL); conf_build_cmd(argc, argv); build_main(argc, argv); std::cout << hex_to_ansi(_code == 0 ? COLOR_SUCCESS : COLOR_ERROR, &_arn, false).pointer << " CODE " << _code << RESET <<  std::endl; conf_build_end(); return _code; }
