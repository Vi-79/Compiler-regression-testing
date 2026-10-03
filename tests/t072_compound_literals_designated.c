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

struct P { int x, y; };
static int dist2(struct P a, struct P b) { int dx = a.x - b.x, dy = a.y - b.y; return dx * dx + dy * dy; }
int main(void) {
    long long s = 0;
    for (int i = 0; i < 1000000; i++)
        s += dist2((struct P){i % 100, (i * 7) % 100}, (struct P){.y = i % 13, .x = (i * 3) % 50});
    int arr[10] = {[3] = 7, [7] = 9, [0] = 1};
    int t = 0;
    for (int i = 0; i < 10; i++) t += arr[i] * (i + 1);
    printf("%lld %d\n", s, t);
    return 0;
}
