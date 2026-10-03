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

#define W 50000
#define BITS 17
#define TS (1 << BITS)
static unsigned long long table[TS];
static unsigned char used[TS];
int main(void) {
    int distinct = 0;
    for (int w = 0; w < W; w++) {
        char c[6];
        for (int k = 0; k < 6; k++) c[k] = (char)('a' + rnd() % 8);
        for (int i = 1; i < 6; i++) { char key = c[i]; int j = i - 1; while (j >= 0 && c[j] > key) { c[j + 1] = c[j]; j--; } c[j + 1] = key; }
        unsigned long long sig = 0;
        for (int k = 0; k < 6; k++) sig |= (unsigned long long)(unsigned char)c[k] << (8 * k);
        unsigned idx = (unsigned)((sig * 0x9E3779B97F4A7C15ULL) >> (64 - BITS));
        while (used[idx] && table[idx] != sig) idx = (idx + 1) & (TS - 1);
        if (!used[idx]) { used[idx] = 1; table[idx] = sig; distinct++; }
    }
    printf("distinct=%d\n", distinct);
    return 0;
}
