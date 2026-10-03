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

#define N 300000
static char text[N + 1], enc[N + 1], dec[N + 1];
int main(void) {
    for (int i = 0; i < N; i++) text[i] = (char)('a' + rnd() % 26);
    for (int i = 0; i < N; i++) enc[i] = (char)('a' + (text[i] - 'a' + 7) % 26);
    for (int i = 0; i < N; i++) dec[i] = (char)('a' + (enc[i] - 'a' + 19) % 26);
    unsigned long long h = 0;
    for (int i = 0; i < N; i++) h = h * 33ULL + (unsigned char)enc[i];
    printf("roundtrip=%d hash=%llu\n", memcmp(text, dec, N) == 0, h);
    return 0;
}
