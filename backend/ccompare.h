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
