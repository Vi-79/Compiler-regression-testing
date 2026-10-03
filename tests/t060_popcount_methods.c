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

static unsigned char tbl[256];
static int pc_loop(uint32_t x) { int c = 0; while (x) { x &= x - 1; c++; } return c; }
static int pc_tbl(uint32_t x) { return tbl[x & 255] + tbl[(x >> 8) & 255] + tbl[(x >> 16) & 255] + tbl[x >> 24]; }
int main(void) {
    for (int i = 0; i < 256; i++) tbl[i] = (unsigned char)pc_loop((uint32_t)i);
    unsigned long long a = 0, b = 0, c = 0;
    for (uint32_t i = 0; i < 5000000; i++) {
        uint32_t v = i * 2654435761u;
        a += (unsigned)pc_loop(v); b += (unsigned)pc_tbl(v); c += (unsigned)__builtin_popcount(v);
    }
    printf("loop=%llu table=%llu builtin=%llu\n", a, b, c);
    return 0;
}
