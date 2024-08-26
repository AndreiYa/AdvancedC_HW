void btUpView(tree *root) {
    if (root == NULL)
        return;

    int hdMap[1000];
    for (int i = 0; i < 1000; i++) {
        hdMap[i] = INT_MIN;
    }

    Queue* q = createQueue();
    enqueue(q, root, 500);

    while (!isQueueEmpty(q)) {
        QueueNode* temp = dequeue(q);
        int hd = temp->data.hd;
        tree* node = temp->data.node;

        if (hdMap[hd] == INT_MIN) {
            hdMap[hd] = node->key;
        }

        if (node->left) {
            enqueue(q, node->left, hd - 1);
        }
        if (node->right) {
            enqueue(q, node->right, hd + 1);
        }

        free(temp);
    }
}
