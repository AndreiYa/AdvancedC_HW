#include <stdio.h>

int main(void)
{
    int N;
    scanf("%d", &N);

    if (N <= 0)
    {
        return 1;
    }

    int num;
    int max = -2147483648;
    int counter = 0;

    for(int i = 0; i < N; i++)
    {
        scanf("%d", &num);
        if (num > max)
        {
            max = num;
            counter = 1;
        } else if (num == max)
        {
            counter++;
        }
    }

    printf("%d\n", counter);
    return 0;
}