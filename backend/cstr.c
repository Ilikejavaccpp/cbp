#include "../backend/cstr.h"
#include "../backend/calias.h"

#include <malloc.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>



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
