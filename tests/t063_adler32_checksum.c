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

#define N 4000000
static unsigned char buf[N];
int main(void) {
    for (int i = 0; i < N; i++) buf[i] = (unsigned char)(rnd() & 0xFF);
    uint32_t a = 1, b = 0;
    for (int i = 0; i < N; i++) { a = (a + buf[i]) % 65521u; b = (b + a) % 65521u; }
    printf("adler32=%08x\n", (b << 16) | a);
    return 0;
}
