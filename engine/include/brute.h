/*
 * brute.h - exact top-k search by checking every vector (the baseline).
 *
 * Used to measure how accurate HNSW is (recall@10) and how much work it
 * saves. Keeps the k best seen so far in a max-heap of size k: the worst
 * of the k sits on top, so a closer vector replaces it in O(log k).
 * Total cost: O(n * dim) distances + O(n log k) heap work.
 *
 * Ties on distance are broken by the smaller id, so the result is
 * the same every run.
 */
#ifndef BRUTE_H
#define BRUTE_H

#include <stddef.h>
#include <stdint.h>

#include "heap.h"
#include "vecstore.h"

/*
 * Finds the k vectors in vs closest to query (squared L2 distance).
 *
 * out must have room for k items. It is filled closest first.
 * Returns how many results were written: min(k, number of vectors),
 * or -1 if memory allocation fails.
 *
 * Every distance computed is counted by the distance counter
 * (see distance.h), so callers can report "compared".
 */
int brute_search(const VecStore *vs, const float *query, size_t k,
                 HeapItem *out);

#endif /* BRUTE_H */
