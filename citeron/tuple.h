/*
    ===[ INFO ]===
    Ever wanted to put generic data next to each other in one
    big blob of `void*`? This does that.
     - `sized_t` makes operations on typeless data *significantly* 
        easier. Most of this library demands `sized_t`s.
     - `tuple_t` is essentially a refactored array of `sized_t`s,
        with the data squashed together and indexing info outside.  
*/

#ifndef CIT_TUPLE_H
#define CIT_TUPLE_H

#include "common.h"
#include "mem.h"

/* Generic sized data. Useful for `void*` inputs. */
typedef struct sized_t {
    void*  data;
    size_t size;
} sized_t;

/* Generic adjacent objects (Python's tuples if they weren't const). */
typedef struct tuple_t {
    sized_t data;
    size_t* sizes;
    size_t* offsets;
    size_t  n;
} tuple_t;

extern tuple_t tuple_from(allocator_t* allocator, size_t num, ...);
extern void    tuple_free(allocator_t* allocator, tuple_t* tup);
extern void*   tuple_get(tuple_t* tup, size_t index);
extern sized_t tuple_get_sized(tuple_t* tup, size_t index);

#endif