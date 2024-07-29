void *findMaxBlock(list *head) {
    if (head == NULL) {
        return NULL;
    }

    void *maxAddress = head->address;
    size_t maxSize = head->size;
    list *current = head->next;

    while (current != NULL) {
        if (current->size > maxSize) {
            maxSize = current->size;
            maxAddress = current->address;
        }
        current = current->next;
    }

    return maxAddress;
}
