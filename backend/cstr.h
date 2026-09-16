// use verison instead like
// `20260805L`

#ifndef CEN__STR_H
#define CEN__STR_H 1

#define CEN__STR_VERSION 20260805L
#define CEN__STR_VERSION__UNSTABLE 20260907L

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "ccompare.h"
#include "calias.h"

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
