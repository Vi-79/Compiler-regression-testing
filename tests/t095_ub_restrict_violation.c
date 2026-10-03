#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <limits.h>
#include <ctype.h>

/* xorshift64 pseudo random generator (unsigned arithmetic only, fully defined) */
static uint64_t rng_state = 88172645463325252ULL;
static uint32_t rnd(void) {
    rng_state ^= rng_state << 13;
    rng_state ^= rng_state >> 7;
    rng_state ^= rng_state << 17;
    return (uint32_t)(rng_state >> 11);
}

static void add_first(int *restrict a, int *restrict b, int n) {
    for (int i = 0; i < n; i++) a[i] += b[0];
}
int main(void) {
    int arr[8] = {1, 1, 1, 1, 1, 1, 1, 1};
    add_first(arr, arr, 8);
    for (int i = 0; i < 8; i++) printf("%d ", arr[i]);
    printf("\n");
    return 0;
}
