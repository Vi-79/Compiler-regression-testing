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

#define N 500000
static char text[N + 1];
int main(void) {
    for (int i = 0; i < N; i++) {
        unsigned r = rnd() % 10;
        text[i] = r < 5 ? (char)('a' + rnd() % 26) : r < 7 ? (char)('0' + rnd() % 10) : r < 9 ? ' ' : '.';
    }
    int i = 0, words = 0, nums = 0, other = 0;
start:
    if (i >= N) goto done;
    if (isalpha((unsigned char)text[i])) goto in_word;
    if (isdigit((unsigned char)text[i])) goto in_num;
    other++; i++;
    goto start;
in_word:
    words++;
    while (i < N && isalpha((unsigned char)text[i])) i++;
    goto start;
in_num:
    nums++;
    while (i < N && isdigit((unsigned char)text[i])) i++;
    goto start;
done:
    printf("words=%d numbers=%d other=%d\n", words, nums, other);
    return 0;
}
