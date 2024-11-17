
#ifndef CITERON_H
#define CITERON_H

/* ====[ FILE: common.h ]==== */

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

/* ====[ FILE: mem.h ]==== */

/*
    ===[ INFO ]===
    Custom allocator support for the entire library. 
     - `context` or `ctx` is for stateless allocators. 
     - `ptrdiff_t` sizes make some math consistent.
    
    If you don't specify an allocator when asked (`NULL`), 
    most of this library will assume you meant the regular
    `malloc`/`free`, which will be used as fallback via
    the singleton `lib_allocator`.
*/

/* Custom allocator interface. */
typedef struct allocator_t {
    void* (*malloc) (            ptrdiff_t size, void* ctx);
    void  (*free)   (void* data, ptrdiff_t size, void* ctx);
    void   *context;
} allocator_t;

/* Default allocator. */
extern allocator_t lib_allocator;

/* Use an allocator like a normal `malloc` function (`ptrdiff_t size`). */
#define aloc_malloc(aloc_ptr, size) \
    (aloc_ptr)->malloc(((ptrdiff_t)(size)), (aloc_ptr)->context)

/* Just `aloc_malloc` with backup (see `aloc_backup`). */
#define aloc_malloc_backup(aloc_ptr, size) \
    aloc_malloc(aloc_backup(aloc_ptr), size)
    
/* Use an allocator like a normal `free` function (`ptrdiff_t size`). */
#define aloc_free(data, aloc_ptr, size) \
    do { (aloc_ptr)->free(data, ((ptrdiff_t)(size)), (aloc_ptr)->context); data = NULL; } while (0)
    
/* Just `aloc_free` with backup (see `aloc_backup`). */
#define aloc_free_backup(data, aloc_ptr, size) \
    aloc_free(data, aloc_backup(aloc_ptr), size)

/* Ensures an allocator is non-`NULL` by assigning `lib_allocator` if necessary. Use as freestanding statement. */
#define aloc_ensure(aloc_ptr) \
    if ((aloc_ptr) == NULL) { (aloc_ptr) = &lib_allocator; }

/* Use `aloc_ptr` (unmodified) if available or `lib_allocator` as backup. Use as in-line. */
#define aloc_backup(aloc_ptr) \
    ((aloc_ptr) == NULL ? &lib_allocator : (aloc_ptr))

/* ====[ FILE: tuple.h ]==== */

/*
    ===[ INFO ]===
    Ever wanted to put generic data next to each other in one
    big blob of `void*`? This does that.
     - `sized_t` makes operations on typeless data *significantly* 
        easier. Most of this library demands `sized_t`s.
     - `tuple_t` is essentially a refactored array of `sized_t`s,
        with the data squashed together and indexing info outside.  
*/

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

#ifdef CITERON_IMPLEMENTATION

/* ====[ FILE: mem.c ]==== */

static void* lib_malloc(ptrdiff_t size, void* context) {
    ignore(context);
    ignore(size);

    return malloc(size);
}

static void lib_free(void* data, ptrdiff_t size, void* context){
    ignore(context);
    ignore(size);

    free(data);
}

allocator_t lib_allocator = {
    .malloc  = lib_malloc,
    .free    = lib_free,
    .context = NULL
};
/* ====[ FILE: tuple.c ]==== */

/*
    Create a generic tuple of adjacent objects.
    Variadic arguments must be `sized_t`!

    @param `allocator` The allocator to use. NULL if default.
    @param `num` The number of arguments.
    @param `...` The `sized_t` generic objects to put into the tuple.

    @returns New `tuple_t` with the sized data from the varargs.

    @note Copies data into the tuple!
*/
tuple_t tuple_from(allocator_t* allocator, size_t num, ...) {
    aloc_ensure(allocator);

    /* Varargs setup. */
    va_list args, args_sizes;
    va_start(args, num);
    va_copy(args_sizes, args); /* You can't re-iterate `args`, hence the copy. */
    
    /* Func-wide variables. */
    sized_t elem;
    tuple_t tup = {
        .n = num,
        .sizes = aloc_malloc(allocator, num * sizeof(size_t)),
        .offsets = aloc_malloc(allocator, num * sizeof(size_t)),
    };
    
    /* Get sizes first (avoids realloc). */
    size_t total_size = 0;
    for (size_t i = 0; i < num; i++) {
        elem = va_arg(args_sizes, sized_t);
        tup.offsets[i] = total_size;
        tup.sizes[i] = elem.size;
        total_size += elem.size;
    }

    /* Insert the data. */
    tup.data = (sized_t){
        .data = aloc_malloc(allocator, total_size), 
        .size = total_size
    };

    for (size_t i = 0; i < num; i++) {
        elem = va_arg(args, sized_t);
        memcpy(tup.data.data + tup.offsets[i], elem.data, elem.size);
    }

    /* Varargs teardown. */
    va_end(args);

    return tup;
}

/*
    Frees a tuple.

    @param `allocator` The allocator to use. NULL if default.
    @param `tup` The tuple to free.

    @note Only frees the copy(ies) of the data in the tuple.
*/
void tuple_free(allocator_t* allocator, tuple_t* tup) {
    aloc_ensure(allocator);

    /* Free. */
    aloc_free(tup->data.data, allocator, tup->data.size); 
    aloc_free(tup->offsets,   allocator, sizeof(size_t) * tup->n);
    aloc_free(tup->sizes,     allocator, sizeof(size_t) * tup->n); 
}

/*
    Gets a pointer to the `index`th(/nd/rd) contained object.
    Just cast the returned `void*` to the type you know it to be.

    @param `tup` The tuple to index into.
    @param `index` The index of the object's data you want.

    @returns Pointer to the object's data. 
    
    @note Directly castable.
*/
inline void* tuple_get(tuple_t* tup, size_t index) {
    return tup->data.data + tup->offsets[index];
}

/*
    If `tuple_get` returned a `sized_t`.

    @param `tup` The tuple to index into.
    @param `index` The index of the object's data you want.

    @returns Sized pointer (`sized_t`) to the object's data. 
    
    @note Not directly castable. Go to `sized_t::data` first.
*/
inline sized_t tuple_get_sized(tuple_t* tup, size_t index) {
    return (sized_t) {
        .data = tup->data.data + tup->offsets[index], 
        .size = tup->sizes[index]
    };
}

#endif
#endif
