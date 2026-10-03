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

typedef int (*op_fn)(int, int);
static int op_add(int a, int b) { return a + b; }
static int op_sub(int a, int b) { return a - b; }
static int op_mul(int a, int b) { return a * b; }
static int op_max(int a, int b) { return a > b ? a : b; }
static int op_xor(int a, int b) { return a ^ b; }
int main(void) {
    op_fn ops[5] = {op_add, op_sub, op_mul, op_max, op_xor};
    int acc = 1;
    for (int i = 0; i < 3000000; i++) {
        acc = ops[i % 5](acc, i & 15) % 100003;
        if (acc < 0) acc += 100003;
    }
    printf("%d\n", acc);
    return 0;
}
