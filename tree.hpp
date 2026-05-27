#ifndef TREE_HPP
#define TREE_HPP

#include <vector>
#include <iostream>
#include <string>
#include <unordered_map>
#include <mutex>

struct XmlNodeData {
    int id;
    std::string tag;
    std::string text_content;
};

class Tree {
private:
    struct Node {
        XmlNodeData data;
        Node* parent;
        std::vector<Node*> children;

        Node(XmlNodeData&& value, Node* p = nullptr);
    };

    Node* rootNode;
    int treeSize;
    int k;
    std::unordered_map<int, Node*> node_directory;

    std::mutex tree_mutex;

    void preOrder(Node* node, std::vector<int>& result);
    void postOrder(Node* node, std::vector<int>& result);
    void deleteSubtree(Node* node); //si se borra un nodo, se borran todos sus hijos y luego ese nodo.
    void printTree(Node* node, int depth);

public:
    Tree(int k);

    bool isEmpty();
    int size();

    int root();

    int parent(int id);
    std::vector<int> children(int id);

    bool insert(int parentId, XmlNodeData&& value);
    bool remove(int id);
    
    // Node* search(Node* node, int id);
    Node* search(int id);

    std::vector<int> preOrder();
    std::vector<int> postOrder();
    std::vector<int> inOrder();
    
    void printTree();
};

#endif