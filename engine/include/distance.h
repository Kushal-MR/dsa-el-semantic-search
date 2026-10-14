/*
 * distance.h - squared Euclidean distance between two vectors.
 *
 * d(a, b) = sum over i of (a[i] - b[i])^2
 *
 * No square root (see docs/protocol.md, section 1): if a < b then a^2 < b^2,
 * so the closest note is the same either way, and skipping sqrt saves time on
 * every comparison.
 *
 * Every call increments a comparison counter. SEARCH and BRUTE report this
 * counter as "compared": the fair measure of how much work a search did,
 * independent of how fast the machine is.
 */
#ifndef DISTANCE_H
#define DISTANCE_H

#include <stddef.h>
#include <stdint.h>

/* Squared Euclidean distance between a and b, each of length dim.
 * Increments the comparison counter by one. */
float l2sq(const float *a, const float *b, size_t dim);

/* Number of distance computations since the last reset. */
uint64_t distance_count(void);

/* Set the comparison counter back to zero (done at the start of each search). */
void distance_reset(void);

#endif /* DISTANCE_H */
