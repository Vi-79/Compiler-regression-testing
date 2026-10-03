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

typedef struct Node { int v; struct Node *next; } Node;
int main(void) {
    const int N = 200000;
    Node *head = NULL;
    for (int i = 0; i < N; i++) { Node *n = malloc(sizeof *n); n->v = (int)(rnd() % 1000); n->next = head; head = n; }
    Node *prev = NULL;
    while (head) { Node *nx = head->next; head->next = prev; prev = head; head = nx; }
    head = prev;
    unsigned long long s = 0; int idx = 1;
    for (Node *p = head; p; p = p->next, idx++) s += (unsigned long long)p->v * (unsigned)idx;
    while (head) { Node *nx = head->next; free(head); head = nx; }
    printf("weighted_sum=%llu\n", s);
    return 0;
}
