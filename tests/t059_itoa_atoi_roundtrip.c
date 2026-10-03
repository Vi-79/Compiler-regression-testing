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

static int my_itoa(unsigned v, char *out) {
    char tmp[12];
    int n = 0;
    do { tmp[n++] = (char)('0' + v % 10); v /= 10; } while (v);
    for (int i = 0; i < n; i++) out[i] = tmp[n - 1 - i];
    out[n] = 0;
    return n;
}
static unsigned my_atoi(const char *s) {
    unsigned v = 0;
    while (*s) v = v * 10 + (unsigned)(*s++ - '0');
    return v;
}
int main(void) {
    unsigned long long total_len = 0;
    unsigned bad = 0;
    for (unsigned i = 0; i < 2000000; i++) {
        unsigned v = i * 2654435761u;
        char b[16];
        total_len += (unsigned)my_itoa(v, b);
        if (my_atoi(b) != v) bad++;
    }
    printf("length=%llu mismatches=%u\n", total_len, bad);
    return 0;
}
