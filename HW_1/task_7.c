#include <stdio.h>

int main(void) {
    unsigned int N;
    scanf("%u", &N);

    unsigned int mask = 0xFF << 24;
    unsigned int result = N ^ mask;

    printf("%u\n", result);

    return 0;
}
