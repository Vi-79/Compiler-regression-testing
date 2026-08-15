#include <stdio.h>
int main(void) {
    int a[] = {4, 8, 15, 16, 23, 42};
    int sum = 0;
    for (int i = 0; i < 6; i++)
        sum += a[i];
    printf("%d\n", sum);
    return 0;
}
