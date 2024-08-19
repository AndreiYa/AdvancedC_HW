#include <stdio.h>

int count_zero(int number) {
    int count = 0;
    while (number > 0) {
        if ((number & 1) == 0) {
            count++;
        }
        number >>= 1;
    }
    return count;
}

int main() {
    int N, K;
    int result = 0;
    scanf("%d %d", &N, &K);

    for (int i = 1; i <= N; i++) {
        if (count_zero(i) == K) {
            result++;
        }
    }

    printf("%d\n", result);
    return 0;
}
