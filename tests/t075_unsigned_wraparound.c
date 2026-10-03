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
    uint8_t a = 250; uint16_t b = 65530; uint32_t c = 4294967290u; uint64_t d = 18446744073709551610ULL;
    unsigned long long acc = 0;
    for (int i = 0; i < 20; i++) { a += 3; b += 5; c += 7; d += 9; acc += a + b + c + d; }
    printf("%u %u %u %llu %llu\n", a, b, c, (unsigned long long)d, acc);
    uint32_t h = 17;
    for (int i = 0; i < 5000000; i++) h = h * 1664525u + 1013904223u;
    printf("%u\n", h);
    return 0;
}
