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

static uint32_t rotl(uint32_t x, int r) { return (x << r) | (x >> (32 - r)); }
int main(void) {
    uint32_t h0 = 0x67452301u, h1 = 0xEFCDAB89u, h2 = 0x98BADCFEu, h3 = 0x10325476u;
    for (uint32_t block = 0; block < 300000; block++) {
        uint32_t w = block * 2654435761u;
        for (int r = 0; r < 16; r++) {
            h0 += rotl(h1 ^ w, 5) + h2;
            h1 = rotl(h1, 7) ^ (h0 + w);
            h2 += rotl(h3, 11) ^ h0;
            h3 = rotl(h3 + h1, 13) - h2;
            w = rotl(w, 3) + (uint32_t)r;
        }
    }
    printf("%08x %08x %08x %08x\n", h0, h1, h2, h3);
    return 0;
}
