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

int main(void) {
    int i = 1;
    i = i++ + ++i;
    int a[3] = {0, 0, 0}, j = 0;
    a[j] = j++;
    printf("%d %d %d\n", i, a[0], a[1]);
    return 0;
}
