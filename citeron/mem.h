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

#ifndef CIT_MEMORY_H
#define CIT_MEMORY_H

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

#endif