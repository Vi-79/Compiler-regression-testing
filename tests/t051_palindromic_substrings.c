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

#define N 3000
static char s[N];
int main(void) {
    for (int i = 0; i < N; i++) s[i] = (char)('a' + rnd() % 2);
    long count = 0;
    for (int c = 0; c < 2 * N - 1; c++) {
        int l = c / 2, r = l + (c % 2);
        while (l >= 0 && r < N && s[l] == s[r]) { count++; l--; r++; }
    }
    printf("palindromes=%ld\n", count);
    return 0;
}
