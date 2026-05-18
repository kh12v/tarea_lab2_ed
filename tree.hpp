#include <vector>
#include <iostream>
#include <string>

struct SimilarBook {
    std::string title;
    std::string isbn;
    int publication_year;
};

struct Book {
    std::string id;
    std::string title;
    std::string isbn;
    int publication_year;
    std::string language_code;
    std::string description;
    float average_rating;
    int num_pages;
    std::vector<SimilarBook> similar_books;
};

class Tree {
private:
    struct Node {
        Book data;
        Node* parent;
        std::vector<Node*> children;

        Node(Book value, Node* p = nullptr);
    };

    Node* rootNode;
    int treeSize;
    int k;

    void preOrder(Node* node, std::vector<std::string>& result);
    void postOrder(Node* node, std::vector<std::string>& result);
    void deleteSubtree(Node* node); //si se borra un nodo, se borran todos sus hijos y luego ese nodo.

public:
    Tree(int k);

    bool isEmpty();
    int size();

    std::string root();

    std::string parent(std::string id);
    std::vector<std::string> children(std::string id);

    bool insert(std::string parentId, Book value);
    bool remove(std::string id);
    
    Node* search(Node* node, std::string id);

    std::vector<std::string> preOrder();
    std::vector<std::string> postOrder();
    std::vector<std::string> inOrder();
};