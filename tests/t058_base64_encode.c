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

#define N 300001
static unsigned char in[N];
static char out[4 * (N / 3 + 2) + 1];
int main(void) {
    static const char T[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    for (int i = 0; i < N; i++) in[i] = (unsigned char)(rnd() & 0xFF);
    int o = 0;
    for (int i = 0; i < N; i += 3) {
        unsigned v = (unsigned)in[i] << 16;
        if (i + 1 < N) v |= (unsigned)in[i + 1] << 8;
        if (i + 2 < N) v |= (unsigned)in[i + 2];
        out[o++] = T[(v >> 18) & 63];
        out[o++] = T[(v >> 12) & 63];
        out[o++] = (i + 1 < N) ? T[(v >> 6) & 63] : '=';
        out[o++] = (i + 2 < N) ? T[v & 63] : '=';
    }
    out[o] = 0;
    unsigned long long h = 0;
    for (int i = 0; i < o; i++) h = h * 257ULL + (unsigned char)out[i];
    printf("length=%d hash=%llu tail=%s\n", o, h, out + o - 8);
    return 0;
}
