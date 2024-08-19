#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

bool is_unique(int num, int *unique_numbers, int size) {
    for (int i = 0; i < size; i++) {
        if (unique_numbers[i] == num) {
            return false;
        }
    }
    return true;
}

int main() {
    char N[20];
    scanf("%s", N);

    int length = strlen(N);
    int unique_numbers[1000];
    int count = 0;

    for (int i = 0; i < length - 2; i++) {
        for (int j = i + 1; j < length - 1; j++) {
            for (int k = j + 1; k < length; k++) {
                if (N[i] == '0') {
                    continue;
                }
                int num = (N[i] - '0') * 100 + (N[j] - '0') * 10 + (N[k] - '0');

                if (is_unique(num, unique_numbers, count)) {
                    unique_numbers[count++] = num;
                }
            }
        }
    }

    printf("%d\n", count);
    return 0;
}
