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
    unsigned char a = 200, b = 100;
    int r1 = a + b;
    unsigned char r2 = (unsigned char)(a + b);
    short s = -1; unsigned u = 1;
    printf("%d %d %d %d\n", r1, r2, s < u, (short)s < (int)u);
    unsigned long long acc = 0;
    for (int i = 0; i < 256; i++)
        for (int j = 0; j < 256; j++) {
            unsigned char x = (unsigned char)i, y = (unsigned char)j;
            acc += (unsigned)(x + y) + (unsigned)(unsigned char)(x * y) + (unsigned)(x << 4) + (unsigned)(x >> 2);
            acc += (unsigned)((signed char)x < (signed char)y);
        }
    printf("%llu\n", acc);
    return 0;
}
