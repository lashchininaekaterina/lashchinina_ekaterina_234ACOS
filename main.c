#include <stdio.h>

void swap(int *x1, int *y2) {
    int z;
    z = *x1;
    *x1 = *y2;
    *y2 = z;
}

int main() {
    int x, y;
    scanf("%d%d", &x, &y);
    swap(&x, &y);
    printf("%d %d", x, y);
    return 0;
}