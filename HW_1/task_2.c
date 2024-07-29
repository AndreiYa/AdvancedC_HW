#include <stdio.h>
#include <ctype.h>

int main(void)
{
    int N;
    char ch;
    scanf("%d\n", &N);

    while ((ch = getchar()) != '.')
    {
        if (isalpha(ch))
        {
            char base;
            if (islower(ch))
            {
                base = 'a';
                ch = (ch - base + N) % 26 + base;
            } else if (isupper(ch))
            {
                base = 'A';
                ch = (ch - base + N) % 26 + base;
            }
        }
        putchar(ch);
    }
    putchar('.');

    return 0;
}
