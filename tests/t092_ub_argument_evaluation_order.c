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

static int counter = 0;
static int next(void) { return ++counter; }
static void show(int a, int b, int c) { printf("%d %d %d\n", a, b, c); }
int main(void) {
    show(next(), next(), next());
    printf("%d\n", next() * 10 + next());
    return 0;
}
