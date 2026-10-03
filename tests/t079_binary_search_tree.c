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

typedef struct T { int key; struct T *l, *r; } T;
static T *insert(T *root, int key) {
    T **pp = &root;
    while (*pp) { if (key < (*pp)->key) pp = &(*pp)->l; else if (key > (*pp)->key) pp = &(*pp)->r; else return root; }
    *pp = calloc(1, sizeof(T)); (*pp)->key = key;
    return root;
}
static int height(T *t) { if (!t) return 0; int a = height(t->l), b = height(t->r); return 1 + (a > b ? a : b); }
static unsigned long long pos_sum; static int counter;
static void inorder(T *t) { if (!t) return; inorder(t->l); pos_sum += (unsigned long long)t->key * (unsigned)(++counter); inorder(t->r); }
static void destroy(T *t) { if (!t) return; destroy(t->l); destroy(t->r); free(t); }
int main(void) {
    T *root = NULL;
    for (int i = 0; i < 100000; i++) root = insert(root, (int)(rnd() % 1000000));
    inorder(root);
    printf("nodes=%d height=%d sum=%llu\n", counter, height(root), pos_sum);
    destroy(root);
    return 0;
}
