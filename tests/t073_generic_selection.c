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

#define NAME(x) _Generic((x), int: "int", unsigned: "unsigned", long: "long", float: "float", double: "double", char: "char", default: "other")
#define HALF(x) _Generic((x), int: (x) / 2, double: (x) / 2.0, float: (x) / 2.0f, default: (x))
int main(void) {
    int i = 7; unsigned u = 7u; long l = 7L; float f = 7.0f; double d = 7.0; char c = 'x';
    printf("%s %s %s %s %s %s %s\n", NAME(i), NAME(u), NAME(l), NAME(f), NAME(d), NAME(c), NAME(1 + 'a'));
    double acc = 0.0;
    for (int k = 0; k < 1000000; k++) acc += (double)HALF(k) + HALF((double)k) + (double)HALF((float)k);
    printf("%.1f\n", acc);
    return 0;
}
