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

static uint32_t rev_loop(uint32_t x) { uint32_t r = 0; for (int i = 0; i < 32; i++) { r = (r << 1) | (x & 1u); x >>= 1; } return r; }
static uint32_t rev_swap(uint32_t x) {
    x = ((x >> 1) & 0x55555555u) | ((x & 0x55555555u) << 1);
    x = ((x >> 2) & 0x33333333u) | ((x & 0x33333333u) << 2);
    x = ((x >> 4) & 0x0F0F0F0Fu) | ((x & 0x0F0F0F0Fu) << 4);
    x = ((x >> 8) & 0x00FF00FFu) | ((x & 0x00FF00FFu) << 8);
    return (x >> 16) | (x << 16);
}
int main(void) {
    unsigned long long s1 = 0, s2 = 0;
    unsigned bad = 0;
    for (uint32_t i = 0; i < 2000000; i++) {
        uint32_t v = i * 2246822519u;
        uint32_t a = rev_loop(v), b = rev_swap(v);
        s1 += a; s2 += b;
        if (a != b) bad++;
    }
    printf("sum_loop=%llu sum_swap=%llu mismatches=%u\n", s1, s2, bad);
    return 0;
}
