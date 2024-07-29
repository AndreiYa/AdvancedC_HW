#include <stdio.h>

int main(void) {
    unsigned int N;
    unsigned int K;
    scanf("%u %u", &N, &K);

    unsigned int mask = (1U << K) - 1;
    unsigned int result = N & mask;

    printf("%u\n", result);

    return 0;
}