#include <stdio.h>

int countSetBits(unsigned int n) {
    int count = 0;
    while (n) {
        n &= (n - 1);
        count++;
    }
    return count;
}

int main(void) {
    unsigned int N;
    scanf("%u", &N);
    printf("%d\n", countSetBits(N));

    return 0;
}
