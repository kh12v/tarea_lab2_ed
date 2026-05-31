#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <utility>
#include "pugixml/pugixml.hpp"
#include "tree.hpp"

namespace fs = std::filesystem;

void populate_tree(pugi::xml_node pugi_node, Tree& tree, int parent_id, int& id_counter) {
    for (pugi::xml_node child = pugi_node.first_child(); child; child = child.next_sibling()) {
        if (child.type() == pugi::node_element) {
            int current_id = ++id_counter;
            XmlNodeData data;
            data.id = current_id;
            data.tag = child.name();
            
            std::string text = child.child_value();
            size_t start = text.find_first_not_of(" \t\n\r");
            if (start != std::string::npos) {
                size_t end = text.find_last_not_of(" \t\n\r");
                data.text_content = text.substr(start, end - start + 1);
            } else {
                data.text_content = "";
            }

            tree.insert(parent_id, std::move(data));
            
            // Recursively populate children
            populate_tree(child, tree, current_id, id_counter);
        }
    }
}

int main() {
    std::string folder_path = "books_xml_filtered";

    if (!fs::exists(folder_path) || !fs::is_directory(folder_path)) {
        std::cerr << "Error: Folder not found -> " << folder_path << std::endl;
        return 1;
    }

    // 1. Instanciar el árbol ÚNICO fuera del bucle (con mayor capacidad para alojar todo)
    Tree mega_tree(5000); 
    
    // 2. Mantener el contador de IDs fuera para que sea global y único
    int id_counter = 0; 

    // 3. Crear e insertar el Nodo Raíz Global
    int global_root_id = ++id_counter;
    XmlNodeData global_root_data;
    global_root_data.id = global_root_id;
    global_root_data.tag = "root_catalogo"; // Nombre representativo para el contenedor global
    global_root_data.text_content = "Contenedor de todos los libros XML";
    
    mega_tree.insert(0, std::move(global_root_data)); // Se inserta sin padre por ser la raíz absoluta

    std::cout << "Reading XML files from " << folder_path << "..." << std::endl;

    std::vector<fs::path> files;
    for (const auto& entry : fs::directory_iterator(folder_path)) {
        if (entry.path().extension() == ".xml") {
            files.push_back(entry.path());
        }
    }

    int count = 0;
    
    for (const auto& filepath : files) {
        pugi::xml_document doc;
        pugi::xml_parse_result result = doc.load_file(filepath.c_str());

        if (result) {
            pugi::xml_node book_node = doc.child("book");
            
            if (book_node) {
                // 4. El nodo <book> de ESTE archivo ahora será hijo de la raíz global
                int book_root_id = ++id_counter;
                
                XmlNodeData book_root_data;
                book_root_data.id = book_root_id;
                book_root_data.tag = book_node.name(); // "book"
                
                std::string root_text = book_node.child_value();
                size_t start = root_text.find_first_not_of(" \t\n\r");
                if (start != std::string::npos) {
                    size_t end = root_text.find_last_not_of(" \t\n\r");
                    book_root_data.text_content = root_text.substr(start, end - start + 1);
                } else {
                    book_root_data.text_content = "";
                }
                
                // IMPORTANTE: Se inserta pasando 'global_root_id' como el padre
                mega_tree.insert(global_root_id, std::move(book_root_data)); 
                
                // 5. Población recursiva usando el mismo árbol global
                populate_tree(book_node, mega_tree, book_root_id, id_counter);
                
            } else {
                std::cerr << "Error: No se encontro el nodo <book> en " << filepath.filename() << "\n";
            }
        } else {
            std::cerr << "Failed to parse " << filepath.filename() 
                      << " - Error: " << result.description() << std::endl;
        }

        int current_count = ++count;
        if (current_count % 100 == 0) {
            std::cout << "Procesados: " << current_count << "/10000" << std::endl;
        }
    }

    // 6. Imprimir el árbol gigante final una sola vez, cuando ya se procesaron todos los archivos
    std::cout << "\n============================\n";
    std::cout << "ESTRUCUTRA DEL ARBOL CREADA:\n";
    std::cout << "============================\n";
    // mega_tree.printTree();

    // ---------------------------------------
    // Utilizar funciones aquí:

    std::cout << "\n--- Probando listar() ---\n";
    std::vector<int> listados = mega_tree.listar();
    std::cout << "Se han listado " << listados.size() << " elementos.\n";
    if (!listados.empty()) {
        std::cout << "Primeros elementos listados: ";
        for (size_t i = 0; i < 5 && i < listados.size(); ++i) {
            std::cout << listados[i] << " ";
        }
        std::cout << "...\n";
    }
    
    std::cout << "\n--- Probando precursores() ---\n";
    std::vector<int> prec = mega_tree.precursores();
    std::cout << "Se encontraron " << prec.size() << " precursores.\n";
    if (!prec.empty()) {
        std::cout << "Primeros precursores encontrados: ";
        for (size_t i = 0; i < 5 && i < prec.size(); ++i) {
            std::cout << prec[i] << " ";
        }
        std::cout << "...\n";
    }

    std::cout << "\n--- Probando borrar_ratings(3.5) ---\n";
    std::cout << "Tamano del arbol antes de borrar: " << mega_tree.size() << "\n";
    mega_tree.borrar_ratings(3.5);
    std::cout << "Tamano del arbol despues de borrar: " << mega_tree.size() << "\n";

    // ---------------------------------------
    return 0;
}
