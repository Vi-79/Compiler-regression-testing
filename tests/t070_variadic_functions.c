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

#include <stdarg.h>
static long long sum_ints(int n, ...) {
    va_list ap; va_start(ap, n);
    long long s = 0;
    for (int i = 0; i < n; i++) s += va_arg(ap, int);
    va_end(ap);
    return s;
}
static double avg_doubles(int n, ...) {
    va_list ap; va_start(ap, n);
    double s = 0.0;
    for (int i = 0; i < n; i++) s += va_arg(ap, double);
    va_end(ap);
    return s / n;
}
int main(void) {
    long long a = 0; double b = 0.0;
    for (int i = 0; i < 200000; i++) {
        a += sum_ints(1, i) + sum_ints(3, i, 2 * i, 3) + sum_ints(6, 1, 2, 3, 4, 5, i);
        b += avg_doubles(2, (double)i, 0.5) + avg_doubles(4, 1.0, 2.0, 3.0, (double)(i % 7));
    }
    printf("%lld %.3f\n", a, b);
    return 0;
}
