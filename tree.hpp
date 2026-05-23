#ifndef TREE_HPP
#define TREE_HPP

#include <vector>
#include <iostream>
#include <string>

struct XmlNodeData {
    std::string id;
    std::string tag;
    std::string text_content;
};

class Tree {
private:
    struct Node {
        XmlNodeData data;
        Node* parent;
        std::vector<Node*> children;

        Node(XmlNodeData value, Node* p = nullptr);
    };

    Node* rootNode;
    int treeSize;
    int k;

    void preOrder(Node* node, std::vector<std::string>& result);
    void postOrder(Node* node, std::vector<std::string>& result);
    void deleteSubtree(Node* node); //si se borra un nodo, se borran todos sus hijos y luego ese nodo.
    void printTree(Node* node, int depth);

public:
    Tree(int k);

    bool isEmpty();
    int size();

    std::string root();

    std::string parent(std::string id);
    std::vector<std::string> children(std::string id);

    bool insert(std::string parentId, XmlNodeData value);
    bool remove(std::string id);
    
    Node* search(Node* node, std::string id);

    std::vector<std::string> preOrder();
    std::vector<std::string> postOrder();
    std::vector<std::string> inOrder();
    
    void printTree();
};

#endif