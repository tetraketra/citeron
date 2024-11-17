#include "common.h"
#include "mem.h"

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