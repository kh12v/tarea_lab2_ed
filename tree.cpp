#include "tree.hpp"
#include <algorithm>
#include <functional>
#include <stdexcept>

// Node
Tree::Node::Node(XmlNodeData value, Node* p) {
    data = value;
    parent = p;
}

// Constructor
Tree::Tree(int k) {
    this->k = k;
    rootNode = nullptr;
    treeSize = 0;
}

bool Tree::isEmpty() {
    return treeSize == 0;
}

int Tree::size() {
    return treeSize;
}

std::string Tree::root() {
    if (!rootNode) throw std::runtime_error("Árbol vacío");
    return rootNode->data.id;
}

Tree::Node* Tree::search(Node* node, std::string id) {
    if (!node) return nullptr;
    if (node->data.id == id) return node;

    for (auto child : node->children) {
        Node* found = search(child, id);
        if (found) return found;
    }
    return nullptr;
}

bool Tree::insert(std::string parentId, XmlNodeData value) {
    if (!rootNode) {
        rootNode = new Node(value);
        treeSize++;
        return true;
    }

    Node* parentNode = search(rootNode, parentId);
    if (!parentNode) return false;

    if ((int)parentNode->children.size() >= k) return false;

    Node* newNode = new Node(value, parentNode);
    parentNode->children.push_back(newNode);
    treeSize++;
    return true;
}

std::string Tree::parent(std::string id) {
    Node* node = search(rootNode, id);
    if (!node || !node->parent)
        throw std::runtime_error("No tiene padre");

    return node->parent->data.id;
}

std::vector<std::string> Tree::children(std::string id) {
    Node* node = search(rootNode, id);
    std::vector<std::string> result;

    if (!node) return result;

    for (auto child : node->children)
        result.push_back(child->data.id);

    return result;
}

void Tree::deleteSubtree(Node* node) {
    if (!node) return;
    for (auto child : node->children)
        deleteSubtree(child);
    delete node;
}

bool Tree::remove(std::string id) {
    Node* node = search(rootNode, id);
    if (!node) return false;

    if (node == rootNode) {
        deleteSubtree(rootNode);
        rootNode = nullptr;
        treeSize = 0;
        return true;
    }

    Node* parent = node->parent;
    auto& siblings = parent->children;

    siblings.erase(
        std::remove(siblings.begin(), siblings.end(), node),
        siblings.end()
    );

    deleteSubtree(node);
    treeSize--;
    return true;
}

void Tree::preOrder(Node* node, std::vector<std::string>& result) {
    if (!node) return;
    result.push_back(node->data.id);
    for (auto child : node->children)
        preOrder(child, result);
}

std::vector<std::string> Tree::preOrder() {
    std::vector<std::string> result;
    preOrder(rootNode, result);
    return result;
}

void Tree::postOrder(Node* node, std::vector<std::string>& result) {
    if (!node) return;
    for (auto child : node->children)
        postOrder(child, result);
    result.push_back(node->data.id);
}

std::vector<std::string> Tree::postOrder() {
    std::vector<std::string> result;
    postOrder(rootNode, result);
    return result;
}

std::vector<std::string> Tree::inOrder() {
    std::vector<std::string> result;

    std::function<void(Node*)> inorder = [&](Node* node) {
        if (!node) return;

        int half = node->children.size() / 2;

        for (int i = 0; i < half; i++)
            inorder(node->children[i]);

        result.push_back(node->data.id);

        for (size_t i = half; i < node->children.size(); i++)
            inorder(node->children[i]);
    };

    inorder(rootNode);
    return result;
}

void Tree::printTree(Node* node, int depth) {
    if (!node) return;

    for (int i = 0; i < depth; ++i) {
        std::cout << "  ";
    }
    
    // Print a connector for children
    if (depth > 0) std::cout << "|-- ";
    
    std::cout << node->data.tag;
    if (!node->data.text_content.empty()) {
        std::cout << " -> " << node->data.text_content;
    }
    std::cout << "\n";

    for (auto child : node->children) {
        printTree(child, depth + 1);
    }
}

void Tree::printTree() {
    printTree(rootNode, 0);
}