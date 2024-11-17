/*
    ===[ INFO ]===
    `main.c` only exists for testing via the debug scripts `debug.sh`,
    `debug_build.sh`, and `debug_run.sh`. This is not a compiled project.
*/

#include <inttypes.h>
#include <stdio.h>

#include "tuple.h"

int main(void) {
    /* TEST: allocator allocation */
    int8_t* a  = aloc_malloc_backup(NULL, sizeof(int8_t));
    int16_t* b = aloc_malloc_backup(NULL, sizeof(int16_t));
    int32_t* c = aloc_malloc_backup(NULL, sizeof(int32_t));

    assert(a != NULL);
    assert(b != NULL);
    assert(c != NULL);

    *a = (int8_t)1;
    *b = (int16_t)200;
    *c = (int32_t)1234;

    /* TEST: tuple creation */
    tuple_t tup = tuple_from(NULL, 3, 
        (sized_t){.data = a, .size = sizeof(int8_t)},
        (sized_t){.data = b, .size = sizeof(int16_t)},
        (sized_t){.data = c, .size = sizeof(int32_t)}
    );

    assert(memcmp(tup.data.data + tup.offsets[0], a, tup.sizes[0]) == 0);
    assert(memcmp(tup.data.data + tup.offsets[1], b, tup.sizes[1]) == 0);
    assert(memcmp(tup.data.data + tup.offsets[2], c, tup.sizes[2]) == 0);

    /* TEST: tuple access */
    assert(memcmp(tuple_get(&tup, 0), a, tup.sizes[0]) == 0);
    assert(memcmp(tuple_get(&tup, 1), b, tup.sizes[1]) == 0);
    assert(memcmp(tuple_get(&tup, 2), c, tup.sizes[2]) == 0);

    assert(memcmp(tuple_get_sized(&tup, 0).data, a, tuple_get_sized(&tup, 0).size) == 0);
    assert(memcmp(tuple_get_sized(&tup, 1).data, b, tuple_get_sized(&tup, 1).size) == 0);
    assert(memcmp(tuple_get_sized(&tup, 2).data, c, tuple_get_sized(&tup, 2).size) == 0);

    /* TEST: allocator freeing */
    aloc_free_backup(a, NULL, sizeof(a));
    aloc_free_backup(b, NULL, sizeof(b));
    aloc_free_backup(c, NULL, sizeof(c));

    /* TEST: tuple freeing */
    tuple_free(NULL, &tup);
    assert(tup.data.data == NULL);
    assert(tup.offsets == NULL);
    assert(tup.sizes == NULL);

    return 0;
}