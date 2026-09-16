#ifndef COLORS_C_H
#define COLORS_C_H

#include <stdbool.h>
#define CEN__STR_ARENA_SIZE 32

#include "calias.h"
#include "cstr.h"

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

// magenta
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
