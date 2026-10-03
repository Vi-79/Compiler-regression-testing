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

static int grid[81];
static long long nodes;
static int ok(int pos, int v) {
    int r = pos / 9, c = pos % 9;
    for (int i = 0; i < 9; i++) { if (grid[r * 9 + i] == v) return 0; if (grid[i * 9 + c] == v) return 0; }
    int br = r / 3 * 3, bc = c / 3 * 3;
    for (int i = 0; i < 3; i++) for (int j = 0; j < 3; j++) if (grid[(br + i) * 9 + bc + j] == v) return 0;
    return 1;
}
static int solve(int pos) {
    while (pos < 81 && grid[pos]) pos++;
    if (pos == 81) return 1;
    for (int v = 1; v <= 9; v++)
        if (ok(pos, v)) { grid[pos] = v; nodes++; if (solve(pos + 1)) return 1; grid[pos] = 0; }
    return 0;
}
int main(void) {
    const char *puzzles[2] = {
        "530070000600195000098000060800060003400803001700020006060000280000419005000080079",
        "800000000003600000070090200050007000000045700000100030001000068008500010090000400"
    };
    for (int p = 0; p < 2; p++) {
        for (int i = 0; i < 81; i++) grid[i] = puzzles[p][i] - '0';
        nodes = 0;
        int solved = solve(0);
        printf("solved=%d nodes=%lld ", solved, nodes);
        for (int i = 0; i < 81; i++) putchar('0' + grid[i]);
        putchar('\n');
    }
    return 0;
}
