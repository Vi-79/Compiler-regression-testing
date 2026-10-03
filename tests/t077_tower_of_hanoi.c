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

static unsigned long long moves, check;
static void hanoi(int n, int a, int b, int c) {
    if (n == 0) return;
    hanoi(n - 1, a, c, b);
    moves++; check += (unsigned long long)(a * 3 + b);
    hanoi(n - 1, c, b, a);
}
int main(void) {
    hanoi(22, 0, 1, 2);
    printf("moves=%llu check=%llu\n", moves, check);
    return 0;
}
