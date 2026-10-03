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

#include <setjmp.h>
static jmp_buf jb;
static int depth_reached;
static void rec(int d, int limit) {
    depth_reached = d;
    if (d == limit) longjmp(jb, 1);
    rec(d + 1, limit);
}
int main(void) {
    volatile long long total = 0;
    for (volatile int lim = 1; lim <= 2000; lim++) {
        if (setjmp(jb) == 0) rec(0, lim);
        else total += depth_reached;
    }
    printf("%lld\n", (long long)total);
    return 0;
}
