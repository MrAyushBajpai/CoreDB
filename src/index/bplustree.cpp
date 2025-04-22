#include "bplustree.h"
#include <algorithm>

BPlusTree::InternalNode::InternalNode() {
    isLeaf = false;
}

BPlusTree::LeafNode::LeafNode() {
    isLeaf = true;
    next = nullptr;
}

BPlusTree::BPlusTree(int order) : order(order) {
    root = std::make_shared<LeafNode>();
}

void BPlusTree::insert(int key, int value) {
    auto [newKey, newNode] = insertRecursive(root, key, value);
    if (newNode) {
        auto newRoot = std::make_shared<InternalNode>();
        newRoot->keys.push_back(newKey);
        newRoot->children.push_back(root);
        newRoot->children.push_back(newNode);
        root = newRoot;
    }
}

std::pair<int, std::shared_ptr<BPlusTree::Node>>
BPlusTree::insertRecursive(std::shared_ptr<Node> node, int key, int value) {
    if (node->isLeaf) {
        auto leaf = std::static_pointer_cast<LeafNode>(node);
        auto it = std::lower_bound(leaf->keys.begin(), leaf->keys.end(), key);
        int pos = it - leaf->keys.begin();
        leaf->keys.insert(it, key);
        leaf->values.insert(leaf->values.begin() + pos, value);

        if ((int)leaf->keys.size() < order) return {0, nullptr};

        // Split
        auto newLeaf = std::make_shared<LeafNode>();
        int mid = order / 2;
        newLeaf->keys.assign(leaf->keys.begin() + mid, leaf->keys.end());
        newLeaf->values.assign(leaf->values.begin() + mid, leaf->values.end());

        leaf->keys.resize(mid);
        leaf->values.resize(mid);

        newLeaf->next = leaf->next;
        leaf->next = newLeaf;

        return {newLeaf->keys.front(), newLeaf};
    }

    // Internal node
    auto internal = std::static_pointer_cast<InternalNode>(node);
    int idx = std::upper_bound(internal->keys.begin(), internal->keys.end(), key) - internal->keys.begin();
    auto [newKey, newChild] = insertRecursive(internal->children[idx], key, value);

    if (!newChild) return {0, nullptr};

    internal->keys.insert(internal->keys.begin() + idx, newKey);
    internal->children.insert(internal->children.begin() + idx + 1, newChild);

    if ((int)internal->keys.size() < order) return {0, nullptr};

    // Split internal
    auto newInternal = std::make_shared<InternalNode>();
    int mid = order / 2;

    int pushUpKey = internal->keys[mid];

    newInternal->keys.assign(internal->keys.begin() + mid + 1, internal->keys.end());
    newInternal->children.assign(internal->children.begin() + mid + 1, internal->children.end());

    internal->keys.resize(mid);
    internal->children.resize(mid + 1);

    return {pushUpKey, newInternal};
}

std::optional<int> BPlusTree::search(int key) {
    return searchRecursive(root, key);
}

std::optional<int> BPlusTree::searchRecursive(std::shared_ptr<Node> node, int key) {
    if (node->isLeaf) {
        auto leaf = std::static_pointer_cast<LeafNode>(node);
        for (size_t i = 0; i < leaf->keys.size(); ++i)
            if (leaf->keys[i] == key)
                return leaf->values[i];
        return std::nullopt;
    }

    auto internal = std::static_pointer_cast<InternalNode>(node);
    int idx = std::upper_bound(internal->keys.begin(), internal->keys.end(), key) - internal->keys.begin();
    return searchRecursive(internal->children[idx], key);
}
