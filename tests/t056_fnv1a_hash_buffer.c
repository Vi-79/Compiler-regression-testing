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

#define N 5000000
static unsigned char buf[N];
int main(void) {
    for (int i = 0; i < N; i++) buf[i] = (unsigned char)(rnd() & 0xFF);
    unsigned long long h = 1469598103934665603ULL;
    for (int i = 0; i < N; i++) { h ^= buf[i]; h *= 1099511628211ULL; }
    printf("%016llx\n", h);
    return 0;
}
