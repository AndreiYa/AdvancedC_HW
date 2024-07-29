size_t totalMemoryUsage(list *head) {
    if (head == NULL) {
        return 0;
    }

    size_t totalSize = 0;
    list *current = head;

    while (current != NULL) {
        totalSize += current->size;
        current = current->next;
    }

    return totalSize;
}
