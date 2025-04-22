#ifndef BPLUSTREE_H
#define BPLUSTREE_H

#include <vector>
#include <map>
#include <memory>
#include <optional>

class BPlusTree {
public:
    BPlusTree(int order);
    void insert(int key, int value);
    std::optional<int> search(int key);

private:
    struct Node {
        bool isLeaf;
        std::vector<int> keys;
        virtual ~Node() = default;
    };

    struct InternalNode : public Node {
        std::vector<std::shared_ptr<Node>> children;
        InternalNode();
    };

    struct LeafNode : public Node {
        std::vector<int> values;
        std::shared_ptr<LeafNode> next;
        LeafNode();
    };

    int order;
    std::shared_ptr<Node> root;

    void insertInternal(int key, std::shared_ptr<Node> child, std::shared_ptr<InternalNode> parent);
    std::pair<int, std::shared_ptr<Node>> insertRecursive(std::shared_ptr<Node> node, int key, int value);
    std::optional<int> searchRecursive(std::shared_ptr<Node> node, int key);
};

#endif // BPLUSTREE_H
