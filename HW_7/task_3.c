tree* findBrother(tree *root, int key) {
    if (root == NULL) {
        return 0;
    }

    if (root->key == key) {
        if (root->parent == NULL) {
            return 0;
        }

        if (root->parent->left == root) {
            return root->parent->right;
        }
        else if (root->parent->right == root) {
            return root->parent->left;
        }
    }

    tree* leftSearch = findBrother(root->left, key);
    if (leftSearch != 0) {
        return leftSearch;
    }

    return findBrother(root->right, key);
}
