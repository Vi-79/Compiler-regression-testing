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

static const char *p;
static long long expr(void);
static long long factor(void) {
    if (*p == '(') { p++; long long v = expr(); p++; return v; }
    long long v = 0;
    while (*p >= '0' && *p <= '9') { v = v * 10 + (*p - '0'); p++; }
    return v;
}
static long long term(void) {
    long long v = factor();
    while (*p == '*' || *p == '/' || *p == '%') {
        char op = *p++;
        long long r = factor();
        if (op == '*') v = (v * r) % 10007;
        else { if (r == 0) r = 1; v = (op == '/') ? v / r : v % r; }
    }
    return v;
}
static long long expr(void) {
    long long v = term();
    while (*p == '+' || *p == '-') { char op = *p++; long long r = term(); v = (op == '+') ? v + r : v - r; }
    return v;
}
static char *gen(char *o, int depth) {
    int terms = 1 + (int)(rnd() % 3);
    for (int t = 0; t < terms; t++) {
        if (t) *o++ = "+-*/%"[rnd() % 5];
        if (depth < 4 && rnd() % 3 == 0) { *o++ = '('; o = gen(o, depth + 1); *o++ = ')'; }
        else o += sprintf(o, "%d", (int)(rnd() % 1000));
    }
    return o;
}
int main(void) {
    static char buf[16384];
    long long total = 0, longest = 0;
    for (int i = 0; i < 20000; i++) {
        char *e = gen(buf, 0);
        *e = 0;
        p = buf;
        long long v = expr();
        total += v;
        if ((long long)(e - buf) > longest) longest = e - buf;
    }
    printf("total=%lld longest=%lld\n", total, longest);
    return 0;
}
