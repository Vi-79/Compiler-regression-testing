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

struct S { unsigned a : 3; unsigned b : 5; unsigned c : 8; unsigned d : 16; };
int main(void) {
    struct S s;
    unsigned long long acc = 0;
    for (unsigned i = 0; i < 2000000; i++) {
        s.a = i; s.b = i * 3u; s.c = i * 7u; s.d = i * 13u;
        acc += s.a + s.b + s.c + s.d;
    }
    printf("%llu\n", acc);
    return 0;
}
