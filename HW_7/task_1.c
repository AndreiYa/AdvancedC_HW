#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct list {
    char word[20];
    struct list *next;
};

struct list* add_to_list(struct list* head, char* word) {
    struct list* new_node = (struct list*)malloc(sizeof(struct list));
    if (new_node == NULL) {
        printf("Error!\n");
        exit(1);
    }
    strcpy(new_node->word, word);
    new_node->next = NULL;

    if (head == NULL) {
        return new_node;
    }

    struct list* current = head;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = new_node;

    return head;
}

void swap_elements(struct list* a, struct list* b) {
    char temp[20];
    strcpy(temp, a->word);
    strcpy(a->word, b->word);
    strcpy(b->word, temp);
}

void sort_list(struct list* head) {
    if (head == NULL) return;

    struct list* current;
    struct list* next;
    int swapped;

    do {
        swapped = 0;
        current = head;

        while (current->next != NULL) {
            next = current->next;
            if (strcmp(current->word, next->word) > 0) {
                swap_elements(current, next);
                swapped = 1;
            }
            current = current->next;
        }
    } while (swapped);
}

void print_list(struct list* head) {
    struct list* current = head;
    while (current != NULL) {
        printf("%s ", current->word);
        current = current->next;
    }
}

void delete_list(struct list* head) {
    struct list* current = head;
    struct list* next;

    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
}

int main() {
    char input[1000];
    struct list* head = NULL;

    printf("Enter string: ");
    fgets(input, sizeof(input), stdin);

    input[strcspn(input, ".")] = '\0';

    char* token = strtok(input, " ");
    while (token != NULL) {
        head = add_to_list(head, token);
        token = strtok(NULL, " ");
    }

    sort_list(head);
    printf("\nResult:\n");
    print_list(head);

    delete_list(head);

    return 0;
}
