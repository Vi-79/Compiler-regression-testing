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

#define NB 65536
#define MAXE 200000
typedef struct E { unsigned key; struct E *next; } E;
static E pool[MAXE];
static E *bucket[NB];
int main(void) {
    int used = 0, distinct = 0;
    for (int i = 0; i < MAXE; i++) {
        unsigned key = rnd() % 500000;
        unsigned h = (key * 2654435761u) >> 16;
        E *p = bucket[h];
        while (p && p->key != key) p = p->next;
        if (!p) { pool[used].key = key; pool[used].next = bucket[h]; bucket[h] = &pool[used]; used++; distinct++; }
    }
    int hits = 0;
    for (int i = 0; i < 200000; i++) {
        unsigned key = rnd() % 500000;
        unsigned h = (key * 2654435761u) >> 16;
        for (E *p = bucket[h]; p; p = p->next) if (p->key == key) { hits++; break; }
    }
    printf("distinct=%d hits=%d\n", distinct, hits);
    return 0;
}
