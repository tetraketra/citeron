#ifndef CIT_COMMON_H
#define CIT_COMMON_H

#include <stdlib.h>
#include <stddef.h>
#include <assert.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>

#ifndef _CIT_COMBINATORICS_N
#define _CIT_COMBINATORICS_N 4 
#endif

#ifndef _CIT_MAX_SOURCE_ARRAYS_N
#define _CIT_MAX_SOURCE_ARRAYS_N 16
#endif

#define ignore(any)   ((void)(any))

#define countof(arr)  (size_t)(sizeof(arr) / sizeof(*(arr)))
#define lengthof(arr) (countof(arr) - 1)

#undef  assert
#define assert(cond)  while (!(cond)) { __builtin_unreachable(); }
/*     `static_assert(cond)` (lowercase) is available via `assert.h` as of C23. */

/* Generic sized data. Useful for `void*` inputs. */
typedef struct sized_t {
    void*  data;
    size_t size;
} sized_t;

typedef struct context_t {
    struct {

    } next;

    sized_t src;
    sized_t dest;
} context_t;

/* All transformations must be `sized_t` to `sized_t`. */
typedef bool (transform_ft)(sized_t src, sized_t dest);

#endif