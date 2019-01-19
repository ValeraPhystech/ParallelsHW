#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int main() {
    int n = 100;
    int m = 100;
    srand(time(0));
    printf("%d %d\n", n, m);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            printf("%d ", (int)(rand() % 100));
        }
        printf("\n");
    }
}
