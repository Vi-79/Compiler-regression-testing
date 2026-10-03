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

#define N 64
static unsigned char g[2][N][N];
int main(void) {
    for (int r = 0; r < N; r++) for (int c = 0; c < N; c++) g[0][r][c] = (rnd() % 100) < 35;
    int cur = 0;
    for (int gen = 0; gen < 300; gen++) {
        int nx = cur ^ 1;
        for (int r = 0; r < N; r++)
            for (int c = 0; c < N; c++) {
                int n = 0;
                for (int dr = -1; dr <= 1; dr++)
                    for (int dc = -1; dc <= 1; dc++)
                        if (dr || dc) n += g[cur][(r + N + dr) % N][(c + N + dc) % N];
                g[nx][r][c] = (n == 3) || (g[cur][r][c] && n == 2);
            }
        cur = nx;
    }
    int live = 0; unsigned long long cs = 0;
    for (int r = 0; r < N; r++) for (int c = 0; c < N; c++) { live += g[cur][r][c]; cs = cs * 3ULL + g[cur][r][c]; }
    printf("live=%d checksum=%llu\n", live, cs);
    return 0;
}
