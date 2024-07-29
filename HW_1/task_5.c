#include <stdio.h>

int main(void) {
    unsigned int N;
    unsigned int K;
    scanf("%u %u", &N, &K);

    unsigned int max_value = 0;

    for (int i = 0; i <= 32 - K; i++) {
        unsigned int mask = (1U << K) - 1;
        unsigned int value = (N >> i) & mask;

        if (value > max_value) {
            max_value = value;
        }
    }

    printf("%u\n", max_value);
    return 0;
}
