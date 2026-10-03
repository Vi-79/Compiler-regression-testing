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

union U { float f; uint32_t u; };
int main(void) {
    unsigned long long exp_sum = 0, mant = 0;
    for (int i = 1; i <= 1000000; i++) {
        union U v;
        v.f = (float)i * 0.37f;
        exp_sum += (v.u >> 23) & 255u;
        mant ^= v.u & 0x7FFFFFu;
    }
    printf("exp_sum=%llu mantissa_xor=%llu\n", exp_sum, mant);
    return 0;
}
