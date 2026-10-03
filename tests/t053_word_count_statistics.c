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
static char text[N + 1];
int main(void) {
    for (int i = 0; i < N; i++) text[i] = (rnd() % 6 == 0) ? ' ' : (char)('a' + rnd() % 26);
    int words = 0, longest = 0, cur = 0;
    long total = 0;
    int hist[16] = {0};
    for (int i = 0; i <= N; i++) {
        if (i < N && text[i] != ' ') { cur++; }
        else if (cur) {
            words++; total += cur;
            if (cur > longest) longest = cur;
            hist[cur < 15 ? cur : 15]++;
            cur = 0;
        }
    }
    printf("words=%d longest=%d total=%ld\n", words, longest, total);
    for (int i = 1; i < 16; i++) printf("%d:%d ", i, hist[i]);
    printf("\n");
    return 0;
}
