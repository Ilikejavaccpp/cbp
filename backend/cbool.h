#ifndef STDCEN_BOOL_H
#define STDCEN_BOOL_H

#ifdef __cplusplus
using boolean = bool;
#else

typedef _Bool bool;
typedef bool boolean;

#define true 1
#define false 0

#endif

#endif
