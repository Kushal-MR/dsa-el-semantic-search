/*
 * heap.h - binary heap of (distance, id) pairs, stored in an array.
 *
 * One heap type serves two roles in the engine (see docs/design.md, step 2):
 *   - HEAP_MIN: HNSW candidate list, closest unexplored note on top.
 *   - HEAP_MAX: best-k results, the worst of the current best on top,
 *               so it can be dropped in O(log k) when a closer note is found.
 *
 * Array layout: for the item at index i,
 *   parent = (i - 1) / 2, left child = 2i + 1, right child = 2i + 2.
 *
 * Ordering is by distance, then by id, so results are identical on every run:
 *   HEAP_MIN: smaller distance first; on a tie, smaller id first.
 *   HEAP_MAX: larger distance first;  on a tie, larger id first
 *             (so the smaller id survives when the top is dropped).
 *
 * Complexity: push O(log n), pop O(log n), top O(1).
 * Capacity doubles when full (amortised O(1) growth).
 */
#ifndef HEAP_H
#define HEAP_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    float    dist;  /* squared Euclidean distance to the query */
    uint32_t id;    /* note ID = position in the vector store  */
} HeapItem;

typedef enum {
    HEAP_MIN,
    HEAP_MAX
} HeapKind;

typedef struct {
    HeapItem *items;     /* the heap array                   */
    size_t    size;      /* number of items currently stored */
    size_t    capacity;  /* allocated slots in items         */
    HeapKind  kind;      /* min-heap or max-heap             */
} Heap;

/* Create an empty heap. Returns 0 on success, -1 if memory allocation fails. */
int heap_init(Heap *h, HeapKind kind, size_t initial_capacity);

/* Release the heap's memory. Safe to call on a zeroed or already-freed heap. */
void heap_free(Heap *h);

/* Remove all items but keep the allocated memory for reuse. */
void heap_clear(Heap *h);

/* Add an item. Returns 0 on success, -1 if growing the array fails. */
int heap_push(Heap *h, float dist, uint32_t id);

/* Remove the top item into *out. Returns 0 on success, -1 if the heap is empty. */
int heap_pop(Heap *h, HeapItem *out);

/* The top item without removing it, or NULL if the heap is empty. */
const HeapItem *heap_top(const Heap *h);

/* Number of items in the heap. */
size_t heap_size(const Heap *h);

#endif /* HEAP_H */
