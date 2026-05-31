#ifndef TREE_HPP
#define TREE_HPP

#include <vector>
#include <iostream>
#include <string>
#include <unordered_map>

// Estructura que contiene información sobre un nodo.
// id: identificador único del nodo. (valor asignado al crear el nodo)
// tag: etiqueta del nodo. (nombre del nodo, ej. book, title, etc.)
// text_content: contenido de texto del nodo. (texto dentro del nodo)
struct XmlNodeData {
    int id;
    std::string tag;
    std::string text_content;
};

// Clase para representar un árbol general.
class Tree {
private:
    // Estructura de un nodo con múltiples nodos hijos.
    // data: información del nodo.
    // parent: puntero al nodo padre.
    // children: vector de punteros a los nodos hijos.
    struct Node {
        XmlNodeData data;
        Node* parent;
        std::vector<Node*> children;

        Node(XmlNodeData&& value, Node* p = nullptr);
    };

    // Puntero al nodo raíz.
    Node* rootNode;
    // Tamaño del árbol.
    int treeSize;
    // Máximo número de hijos por nodo.
    int k;
    // Directorio que asocia un ID con su nodo correspondiente.
    std::unordered_map<int, Node*> node_directory;

    // Métodos internos auxiliares
    void preOrder(Node* node, std::vector<int>& result);
    void listar(Node* node, std::vector<int>& result);
    void precursores(Node* node, std::vector<int>& result);
    void postOrder(Node* node, std::vector<int>& result);
    void deleteSubtree(Node* node); 
    void printTree(Node* node, int depth);

public:
    Tree(int k);

    bool isEmpty();
    int size();

    // Retorna el ID de la raíz.
    int root();

    int parent(int id);
    std::vector<int> children(int id);

    bool insert(int parentId, XmlNodeData&& value);
    bool remove(int id);
    
    // Busca el nodo según id. O(1) utilizando unordered_map
    Node* search(int id);

    // Retorna el preOrder del árbol.
    std::vector<int> preOrder();
    // Retorna los IDs de los nodos del árbol según recorrido preOrder.
    std::vector<int> listar();
    // Retorna los IDs de los libros que son precursores.
    std::vector<int> precursores();
    // Retorna el postOrder del árbol.
    std::vector<int> postOrder();
    // Retorna el inOrder del árbol.
    std::vector<int> inOrder();
    
    // Imprime el árbol en preOrder.
    void printTree();

    // Borra los ratings de los libros cuyo rating sea menor a r.
    void borrar_ratings(double r);
};

#endif