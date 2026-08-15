#include <stdio.h>
struct Student {
    int marks;
};
int main(void) {
    struct Student s = {95};
    printf("%d\n", s.marks);
    return 0;
}
