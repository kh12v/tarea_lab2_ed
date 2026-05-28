#include "tree.hpp"
#include <algorithm>
#include <functional>
#include <stdexcept>

// Node
Tree::Node::Node(XmlNodeData&& value, Node* p) {
    data = std::move(value);
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

int Tree::root() {
    if (!rootNode) throw std::runtime_error("Árbol vacío");
    return rootNode->data.id;
}

Tree::Node* Tree::search(int id) {
    auto it = node_directory.find(id);
    if (it != node_directory.end()) {
        return it->second;
    }
    return nullptr;
}
// Tree::Node* Tree::search(Node* node, int id) {
//     if (!node) return nullptr;
//     if (node->data.id == id) return node;

//     for (auto child : node->children) {
//         Node* found = search(child, id);
//         if (found) return found;
//     }
//     return nullptr;
// }

bool Tree::insert(int parentId, XmlNodeData&& value) {
    // 1. Bloqueamos el mutex. Ningún otro hilo pasará de esta línea 
    // hasta que el hilo actual termine la función insert.
    std::lock_guard<std::mutex> lock(tree_mutex);

    int new_id = value.id;

    if (!rootNode) {
        rootNode = new Node(std::move(value));
        node_directory[new_id] = rootNode;
        treeSize++;
        return true;
    }

    // Usamos la búsqueda directa O(1)
    auto it = node_directory.find(parentId);
    if (it == node_directory.end()) return false;
    
    Node* parentNode = it->second;

    if ((int)parentNode->children.size() >= k) return false;

    Node* newNode = new Node(std::move(value), parentNode);
    parentNode->children.push_back(newNode);
    
    node_directory[new_id] = newNode;
    treeSize++;
    
    return true;
}
// bool Tree::insert(int parentId, XmlNodeData value) {
//     if (!rootNode) {
//         rootNode = new Node(value);
//         treeSize++;
//         return true;
//     }

//     Node* parentNode = search(rootNode, parentId);
//     if (!parentNode) return false;

//     if ((int)parentNode->children.size() >= k) return false;

//     Node* newNode = new Node(value, parentNode);
//     parentNode->children.push_back(newNode);
//     treeSize++;
//     return true;
// }

int Tree::parent(int id) {
    // Node* node = search(rootNode, id);
    Node* node = search(id);
    if (!node || !node->parent)
        throw std::runtime_error("No tiene padre");

    return node->parent->data.id;
}

std::vector<int> Tree::children(int id) {
    // Node* node = search(rootNode, id);
    Node* node = search(id);
    std::vector<int> result;

    if (!node) return result;

    for (auto child : node->children)
        result.push_back(child->data.id);

    return result;
}

void Tree::deleteSubtree(Node* node) {
    if (!node) return;
    for (auto child : node->children) {
        deleteSubtree(child);
    }
    // Borrar del directorio antes de destruir el nodo
    node_directory.erase(node->data.id);
    delete node;
}
// void Tree::deleteSubtree(Node* node) {
//     if (!node) return;
//     for (auto child : node->children)
//         deleteSubtree(child);
//     delete node;
// }

bool Tree::remove(int id) {
    // 2. También bloqueamos al eliminar, para evitar que un hilo 
    // intente insertar un hijo en un nodo que otro hilo está borrando.
    std::lock_guard<std::mutex> lock(tree_mutex);

    auto it = node_directory.find(id);
    if (it == node_directory.end()) return false;
    
    Node* node = it->second;

    if (node == rootNode) {
        deleteSubtree(rootNode); // deleteSubtree ya borra del node_directory
        rootNode = nullptr;
        treeSize = 0;
        return true;
    }

    Node* parent = node->parent;
    auto& siblings = parent->children;

    // Eliminar el puntero de la lista de hijos del padre
    for (auto it_sib = siblings.begin(); it_sib != siblings.end(); ++it_sib) {
        if (*it_sib == node) {
            siblings.erase(it_sib);
            break;
        }
    }

    deleteSubtree(node);
    treeSize--;
    return true;
}
// bool Tree::remove(int id) {
//     Node* node = search(rootNode, id);
//     if (!node) return false;

//     if (node == rootNode) {
//         deleteSubtree(rootNode);
//         rootNode = nullptr;
//         treeSize = 0;
//         return true;
//     }

//     Node* parent = node->parent;
//     auto& siblings = parent->children;

//     siblings.erase(
//         std::remove(siblings.begin(), siblings.end(), node),
//         siblings.end()
//     );

//     deleteSubtree(node);
//     treeSize--;
//     return true;
// }

void Tree::preOrder(Node* node, std::vector<int>& result) {
    if (!node) return;
    result.push_back(node->data.id);
    for (auto child : node->children)
        preOrder(child, result);
}

std::vector<int> Tree::preOrder() {
    std::vector<int> result;
    preOrder(rootNode, result);
    return result;
}

//primera funcion listar
void Tree::listar(Node* node, std::vector<int>& result) {
    if (!node) return;
    

    if (node->data.tag == "book") {
        result.push_back(node->data.id);
    }
    
    for (auto child : node->children)
        listar(child, result);
}

std::vector<int> Tree::listar() {
    std::vector<int> result;
    listar(rootNode, result);
    return result;
}

void Tree::postOrder(Node* node, std::vector<int>& result) {
    if (!node) return;
    for (auto child : node->children)
        postOrder(child, result);
    result.push_back(node->data.id);
}

std::vector<int> Tree::postOrder() {
    std::vector<int> result;
    postOrder(rootNode, result);
    return result;
}

std::vector<int> Tree::inOrder() {
    std::vector<int> result;

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