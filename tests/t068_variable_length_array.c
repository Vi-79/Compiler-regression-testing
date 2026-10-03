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

static long long sum_vla(int n) {
    int arr[n];
    for (int i = 0; i < n; i++) arr[i] = (i * i) % 1000;
    long long s = 0;
    for (int i = 0; i < n; i++) s += arr[i];
    return s;
}
int main(void) {
    long long total = 0;
    for (int n = 1; n <= 3000; n++) total += sum_vla(n);
    printf("%lld\n", total);
    return 0;
}
