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

#define L 1000000
static char s[L + 8000];
static char stk[8000];
static int check(int n) {
    int sp = 0;
    for (int i = 0; i < n; i++) {
        char c = s[i];
        if (c == '(' || c == '[' || c == '{') stk[sp++] = c;
        else {
            char o = c == ')' ? '(' : c == ']' ? '[' : '{';
            if (sp == 0 || stk[--sp] != o) return i;
        }
    }
    return sp == 0 ? -1 : n;
}
int main(void) {
    int n = 0, depth = 0, maxd = 0;
    static const char open[3] = {'(', '[', '{'}, close[3] = {')', ']', '}'};
    static char kind[8000];
    while (n < L) {
        if (depth == 0 || (depth < 7000 && (rnd() & 1))) { int k = (int)(rnd() % 3); s[n++] = open[k]; kind[depth++] = (char)k; if (depth > maxd) maxd = depth; }
        else { s[n++] = close[(int)kind[--depth]]; }
    }
    while (depth > 0) s[n++] = close[(int)kind[--depth]];
    int r1 = check(n);
    s[n / 2] = (s[n / 2] == ')') ? ']' : ')';
    int r2 = check(n);
    printf("length=%d maxdepth=%d valid=%d corrupted_first_error=%d\n", n, maxd, r1 == -1, r2);
    return 0;
}
