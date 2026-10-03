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

static int run(const char *p, char *out) {
    static unsigned char tape[30000];
    static int jump[512];
    int stack[256], sp = 0, len = (int)strlen(p), outn = 0, ptr = 0;
    memset(tape, 0, sizeof tape);
    for (int i = 0; i < len; i++) {
        if (p[i] == '[') stack[sp++] = i;
        else if (p[i] == ']') { int j = stack[--sp]; jump[i] = j; jump[j] = i; }
    }
    for (int pc = 0; pc < len; pc++) {
        switch (p[pc]) {
            case '>': ptr++; break;
            case '<': ptr--; break;
            case '+': tape[ptr]++; break;
            case '-': tape[ptr]--; break;
            case '.': out[outn++] = (char)tape[ptr]; break;
            case '[': if (!tape[ptr]) pc = jump[pc]; break;
            case ']': if (tape[ptr]) pc = jump[pc]; break;
            default: break;
        }
    }
    out[outn] = 0;
    return outn;
}
int main(void) {
    const char *prog = "++++++++[>++++[>++>+++>+++>+<<<<-]>+>+>->>+[<]<-]>>.>---.+++++++..+++.>>.<-.<.+++.------.--------.>>+.>++.";
    char out[256];
    unsigned long long cs = 0;
    for (int i = 0; i < 400; i++) {
        int n = run(prog, out);
        for (int k = 0; k < n; k++) cs = cs * 131ULL + (unsigned char)out[k];
    }
    run(prog, out);
    printf("%schecksum=%llu\n", out, cs);
    return 0;
}
