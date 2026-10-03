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

#define N 400000
static unsigned char src[N], enc[2 * N], dec[N];
int main(void) {
    int pos = 0;
    while (pos < N) {
        unsigned char v = (unsigned char)(rnd() & 0xFF);
        int run = 1 + (int)(rnd() % 8);
        for (int k = 0; k < run && pos < N; k++) src[pos++] = v;
    }
    int e = 0;
    for (int i = 0; i < N;) {
        int j = i;
        while (j < N && src[j] == src[i] && j - i < 255) j++;
        enc[e++] = (unsigned char)(j - i);
        enc[e++] = src[i];
        i = j;
    }
    int d = 0;
    for (int k = 0; k < e; k += 2) for (int c = 0; c < enc[k]; c++) dec[d++] = enc[k + 1];
    printf("encoded=%d decoded=%d ok=%d\n", e, d, d == N && memcmp(src, dec, N) == 0);
    return 0;
}
