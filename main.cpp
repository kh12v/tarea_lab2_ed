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

std::mutex mega_tree_mutex;

void populate_tree(pugi::xml_node pugi_node, Tree& tree, int parent_id, std::atomic<int>& id_counter) {
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

            {
                std::lock_guard<std::mutex> lock(mega_tree_mutex);
                tree.insert(parent_id, std::move(data));
            }
            
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
    
    // 2. Mantener el contador de IDs fuera para que sea global y único, ahora atómico
    std::atomic<int> id_counter{0}; 

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

    std::atomic<size_t> file_index{0};
    std::atomic<int> count{0};
    int num_threads = std::thread::hardware_concurrency();
    if (num_threads == 0) num_threads = 4;
    std::vector<std::thread> threads;

    auto worker = [&]() {
        while (true) {
            size_t i = file_index.fetch_add(1);
            if (i >= files.size()) break;
            
            const auto& filepath = files[i];
            
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
                    {
                        std::lock_guard<std::mutex> lock(mega_tree_mutex);
                        mega_tree.insert(global_root_id, std::move(book_root_data)); 
                    }
                    
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
                std::cout << "Procesados: " << current_count << std::endl;
            }
        }
    };

    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(worker);
    }

    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }

    // 6. Imprimir el árbol gigante final una sola vez, cuando ya se procesaron todos los archivos
    std::cout << "Count: " << count << std::endl;
    std::cout << "\n============================================\n";
    std::cout << "ESTRUCTURA DEL ARBOL GLOBAL UNIFICADO:\n";
    std::cout << "============================================\n";
    // mega_tree.printTree();

    return 0;
}
