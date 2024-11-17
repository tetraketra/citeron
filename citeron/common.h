#ifndef CIT_COMMON_H
#define CIT_COMMON_H

#include <stdlib.h>
#include <stddef.h>
#include <assert.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>

#define ignore(any)   ((void)(any))

#define countof(arr)  (size_t)(sizeof(arr) / sizeof(*(arr)))
#define lengthof(arr) (countof(arr) - 1)

#undef  assert
#define assert(cond)  while (!(cond)) { __builtin_unreachable(); }
/*     `static_assert(cond)` (lowercase) is available via `assert.h` as of C23. */

#endif