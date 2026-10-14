/*
 * vecstore.h - dynamic array of fixed-length float vectors.
 *
 * All vectors live in ONE contiguous float array, row after row:
 * vector i starts at data[i * dim]. Its position i is also its note ID
 * (see docs/protocol.md, section 7).
 *
 * Why one array instead of one allocation per vector (docs/design.md, step 2):
 *   - cache-friendly: neighbouring numbers are read together;
 *   - a whole vectors file loads with a single fread;
 *   - no per-vector pointers or bookkeeping.
 *
 * When full, capacity doubles (realloc), so adding a vector is O(1) amortised.
 */
#ifndef VECSTORE_H
#define VECSTORE_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    float  *data;      /* count * dim floats, row after row  */
    size_t  dim;       /* numbers per vector (e.g. 384 or 2) */
    size_t  count;     /* vectors currently stored           */
    size_t  capacity;  /* vectors that fit before growing    */
} VecStore;

/* Create an empty store for vectors of length dim.
 * Returns 0 on success, -1 if dim is 0 or memory allocation fails. */
int vecstore_init(VecStore *vs, size_t dim, size_t initial_capacity);

/* Release the store's memory. Safe to call on a zeroed or already-freed store. */
void vecstore_free(VecStore *vs);

/* Copy one vector (dim floats) to the end of the store.
 * On success returns 0 and writes the new vector's ID to *out_id (if not NULL).
 * Returns -1 if growing the array fails. */
int vecstore_add(VecStore *vs, const float *vec, uint32_t *out_id);

/* Pointer to vector id's first number, or NULL if id is out of range. */
const float *vecstore_get(const VecStore *vs, uint32_t id);

/* Number of vectors stored. */
size_t vecstore_count(const VecStore *vs);

#endif /* VECSTORE_H */
