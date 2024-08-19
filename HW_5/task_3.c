#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAX_STACK_SIZE 1000

int stack[MAX_STACK_SIZE];
int top = -1;

void push(int value) {
    if (top >= MAX_STACK_SIZE - 1) {
        printf("Stack overflow\n");
        exit(1);
    }
    stack[++top] = value;
}

int pop() {
    if (top < 0) {
        printf("Stack underflow\n");
        exit(1);
    }
    return stack[top--];
}

int string_to_int(char *str) {
    int result = 0;
    while (*str) {
        if (*str < '0' || *str > '9') {
            printf("Invalid input\n");
            exit(1);
        }
        result = result * 10 + (*str - '0');
        str++;
    }
    return result;
}

int main() {
    char input[1001];
    fgets(input, sizeof(input), stdin);
    input[strlen(input) - 1] = '\0';

    char *token = strtok(input, " ");
    while (token) {
        if (isdigit(token[0])) {
            push(string_to_int(token));
        } else {
            int b = pop();
            int a = pop();
            switch (token[0]) {
                case '+': {
                    push(a + b); break;
                }
                case '-': {
                    push(a - b); break;
                }
                case '*': {
                    push(a * b); break;
                }
                case '/': {
                    push(a / b); break;
                }
                default: {
                    break;
                }
            }
        }
        token = strtok(NULL, " ");
    }

    printf("%d\n", pop());
    return 0;
}
