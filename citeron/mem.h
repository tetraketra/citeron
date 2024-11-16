#ifndef CIT_MEMORY_H
#define CIT_MEMORY_H

#include "stddef.h"

/* Custom allocator interface. */
typedef struct {
    void* (*malloc) (            ptrdiff_t size, void* ctx);
    void  (*free)   (void* data, ptrdiff_t size, void* ctx);
    void   *context;
} allocator_t;

/* Default allocator. */
extern allocator_t lib_allocator;

#endif