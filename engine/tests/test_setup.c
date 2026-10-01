/*
 * Setup check for the C toolchain (step 1).
 *
 * It confirms three things the engine will rely on:
 *   1. float is 4 bytes (vectors are stored as arrays of float),
 *   2. malloc/free work for a vector-sized block,
 *   3. sqrt from math.h links (needed for distances).
 * If this compiles and prints "C toolchain OK", the machine is ready.
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(void)
{
    if (sizeof(float) != 4) {
        fprintf(stderr, "Unexpected float size: %zu bytes\n", sizeof(float));
        return 1;
    }

    const int dim = 384;                      /* size of one note's vector */
    float *v = malloc((size_t)dim * sizeof *v);
    if (v == NULL) {
        fprintf(stderr, "malloc failed\n");
        return 1;
    }

    double sum = 0.0;
    for (int i = 0; i < dim; i++) {
        v[i] = 1.0f;
        sum += (double)v[i] * v[i];
    }
    free(v);

    /* A 384-long vector of ones has length sqrt(384), about 19.6 */
    printf("Vector length check: %.2f (expected 19.60)\n", sqrt(sum));
    printf("C toolchain OK\n");
    return 0;
}
