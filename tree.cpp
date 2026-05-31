#include "tree.hpp"
#include <algorithm>
#include <functional>
#include <stdexcept>

// Constructor del nodo
Tree::Node::Node(XmlNodeData&& value, Node* p) {
    data = std::move(value);
    parent = p;
}

// Constructor del árbol.
Tree::Tree(int k) {
    this->k = k;
    rootNode = nullptr;
    treeSize = 0;
}

// Retorna true si el árbol está vacío, false en caso contrario.
bool Tree::isEmpty() {
    return treeSize == 0;
}

// Retorna el tamaño del árbol.
int Tree::size() {
    return treeSize;
}

// Retorna el ID de la raíz.
int Tree::root() {
    if (!rootNode) throw std::runtime_error("Árbol vacío");
    return rootNode->data.id;
}

// Busca el nodo según id. O(1) utilizando unordered_map
Tree::Node* Tree::search(int id) {
    auto it = node_directory.find(id);
    if (it != node_directory.end()) {
        return it->second;
    }
    return nullptr;
}

// Inserta un nuevo nodo. O(1)
bool Tree::insert(int parentId, XmlNodeData&& value) {
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

// Retorna el padre del nodo según id. O(1)
int Tree::parent(int id) {
    Node* node = search(id);
    if (!node || !node->parent)
        throw std::runtime_error("No tiene padre");

    return node->parent->data.id;
}

// Retorna los hijos del nodo según id. O(c), c número de hijos del nodo
std::vector<int> Tree::children(int id) {
    Node* node = search(id);
    std::vector<int> result;

    if (!node) return result;

    for (auto child : node->children)
        result.push_back(child->data.id);

    return result;
}

// Elimina los nodos de un subtree. O(n), n número de nodos en el subtree
void Tree::deleteSubtree(Node* node) {
    if (!node) return;
    for (auto child : node->children) {
        deleteSubtree(child);
    }
    // Borrar del directorio antes de destruir el nodo
    node_directory.erase(node->data.id);
    delete node;
    treeSize--; 
}

// Elimina un nodo y todos sus hijos. O(m + c), m número de nodos en el subtree, c número de hermanos del nodo
bool Tree::remove(int id) {
    auto it = node_directory.find(id);
    if (it == node_directory.end()) return false;
    
    Node* node = it->second;

    if (node == rootNode) {
        deleteSubtree(rootNode);
        rootNode = nullptr;
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
    return true;
}

// Método auxiliar para preOrder
void Tree::preOrder(Node* node, std::vector<int>& result) {
    if (!node) return;
    result.push_back(node->data.id);
    for (auto child : node->children)
        preOrder(child, result);
}

// Retorna los IDs de los nodos según preOrder. O(n), n número de nodos en el árbol
std::vector<int> Tree::preOrder() {
    std::vector<int> result;
    preOrder(rootNode, result);
    return result;
}

// Método auxiliar para precursores
void Tree::precursores(Node* node, std::vector<int>& result) {
    if (!node) return;

    if (node->data.tag == "book") {
        int year = 0;
        bool has_year = false;
        bool precursor = true;
        bool has_similar = false;

        // Obtiene el año de publicación del libro actual
        for (auto child : node->children) {
            if (child->data.tag == "publication_year") {
                if (!child->data.text_content.empty()) {
                    try {
                        year = std::stoi(child->data.text_content);
                        has_year = true;
                    } catch (...) {}
                }
            }
        }

        // Solamente evalua si el libro actual tiene un año válido
        if (has_year) {
            for (auto child : node->children) {
                if (child->data.tag == "similar_books") {
                    for (auto similar_book : child->children) {
                        has_similar = true;
                        
                        for (auto similar : similar_book->children) {
                            if (similar->data.tag == "publication_year") {
                                if (!similar->data.text_content.empty()) {
                                    try {
                                        int similar_book_year = std::stoi(similar->data.text_content);
                                        // Si un libro similar no a sido publicado en un año posterior, no es precursor
                                        if (similar_book_year <= year) {
                                            precursor = false;
                                        }
                                    } catch (...) {}
                                }
                            }
                        }
                    }
                }
            }
        } else {
            precursor = false;
        }

        // Si tiene un año, tiene libros similares y todos ellos fueron publicados despues entonces es un precursor
        if (has_year && has_similar && precursor) {
            result.push_back(node->data.id);
        }
    }

    for (auto child : node->children)
        precursores(child, result);
}

// Retorna los IDs de los libros que son precursores. O(n), n número de nodos en el árbol
std::vector<int> Tree::precursores() {
    std::vector<int> result;
    precursores(rootNode, result);
    return result;
}


// Método auxiliar para listar
void Tree::listar(Node* node, std::vector<int>& result) {
    if (!node) return;

    if (node->data.tag == "book") {
        int xml_id = -1;
        // Buscar el tag <id> entre los hijos
        for (auto child : node->children) {
            if (child->data.tag == "id" && !child->data.text_content.empty()) {
                try {
                    xml_id = std::stoi(child->data.text_content);
                } catch (...) {}
                break;
            }
        }
        
        // Solo lo agregamos si se encontró un ID válido (esto ignora los libros similares que no tienen ID)
        if (xml_id != -1) {
            result.push_back(xml_id);
        }
    }
    
    for (auto child : node->children)
        listar(child, result);
}

// Retorna los IDs de los libros en preOrder. O(n), n número de nodos en el árbol
std::vector<int> Tree::listar() {
    std::vector<int> result;
    listar(rootNode, result);
    return result;
}

// Borra los ratings de los libros cuyo rating sea menor a r. O(n^2) peor caso, n número de nodos en el árbol
void Tree::borrar_ratings(double r){
    std::vector<int> ids_to_remove;

    for (auto book : rootNode->children){
        for (auto child : book->children){
            if(child->data.tag == "average_rating" && !child->data.text_content.empty()){
                try {
                    double ranking = std::stod(child->data.text_content);
                    if(ranking <= r){
                        ids_to_remove.push_back(book->data.id);
                    }
                } catch (...) {
                    // Ignorar si el string no es un double válido
                }
            }
        }
    }

    // Recorremos el vector de IDs a borrar y llamamos a remove
    for (int id : ids_to_remove) {
        remove(id);
    }
}

// Método auxiliar para postOrder
void Tree::postOrder(Node* node, std::vector<int>& result) {
    if (!node) return;
    for (auto child : node->children)
        postOrder(child, result);
    result.push_back(node->data.id);
}

// Retorna los IDs de los nodos según postOrder. O(n), n número de nodos en el árbol
std::vector<int> Tree::postOrder() {
    std::vector<int> result;
    postOrder(rootNode, result);
    return result;
}

// Retorna los IDs de los nodos según inOrder. O(n), n número de nodos en el árbol
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

// Método auxiliar para imprimir el árbol
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

// Imprime el árbol. O(n), n número de nodos en el árbol
void Tree::printTree() {
    printTree(rootNode, 0);
}