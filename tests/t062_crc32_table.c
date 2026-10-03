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

#define N 4000000
static unsigned char buf[N];
static uint32_t table[256];
int main(void) {
    for (uint32_t i = 0; i < 256; i++) {
        uint32_t c = i;
        for (int k = 0; k < 8; k++) c = (c & 1u) ? (0xEDB88320u ^ (c >> 1)) : (c >> 1);
        table[i] = c;
    }
    for (int i = 0; i < N; i++) buf[i] = (unsigned char)(rnd() & 0xFF);
    uint32_t crc = 0xFFFFFFFFu;
    for (int i = 0; i < N; i++) crc = table[(crc ^ buf[i]) & 0xFFu] ^ (crc >> 8);
    printf("crc32=%08x\n", crc ^ 0xFFFFFFFFu);
    return 0;
}
