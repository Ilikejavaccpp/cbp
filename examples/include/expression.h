#ifdef __cplusplus
    extern "C" {
#endif

#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <stddef.h>

typedef double calc_result_t; /* since there could be complex results */

typedef struct {
    calc_result_t value;
    int error;
} calc_result;

calc_result calc_eval(const char* expr);
calc_result calc_eval_vL(const char* expr); /* evaluate with Lua 5.3 */

#endif

#ifdef __cplusplus
    }
#endif
