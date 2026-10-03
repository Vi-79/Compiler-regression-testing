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

#define N 2000000
static char t[N + 1];
int main(void) {
    const char *p = "abbabaab";
    const int m = 8;
    for (int i = 0; i < N; i++) t[i] = (char)('a' + rnd() % 2);
    int f[8];
    f[0] = 0;
    for (int i = 1, k = 0; i < m; i++) {
        while (k > 0 && p[i] != p[k]) k = f[k - 1];
        if (p[i] == p[k]) k++;
        f[i] = k;
    }
    int kmp = 0;
    for (int i = 0, k = 0; i < N; i++) {
        while (k > 0 && t[i] != p[k]) k = f[k - 1];
        if (t[i] == p[k]) k++;
        if (k == m) { kmp++; k = f[k - 1]; }
    }
    int naive = 0;
    for (int i = 0; i + m <= N; i++) if (memcmp(t + i, p, (size_t)m) == 0) naive++;
    printf("kmp=%d naive=%d\n", kmp, naive);
    return 0;
}
