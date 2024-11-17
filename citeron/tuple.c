#include "tuple.h"

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