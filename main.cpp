#include <iostream>
#include <filesystem>
#include <string>
#include "pugixml/pugixml.hpp"
#include "tree.hpp"

namespace fs = std::filesystem;

void populate_tree(pugi::xml_node pugi_node, Tree& tree, const std::string& parent_id, int& id_counter) {
    for (pugi::xml_node child = pugi_node.first_child(); child; child = child.next_sibling()) {
        if (child.type() == pugi::node_element) {
            std::string current_id = "node_" + std::to_string(++id_counter);
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

            tree.insert(parent_id, data);
            
            // Recursively populate children
            populate_tree(child, tree, current_id, id_counter);
        }
    }
}

int main() {
    std::string folder_path = "books_xml";

    if (!fs::exists(folder_path) || !fs::is_directory(folder_path)) {
        std::cerr << "Error: Folder not found -> " << folder_path << std::endl;
        return 1;
    }

    int count = 0;
    std::cout << "Reading XML files from " << folder_path << "..." << std::endl;

    for (const auto& entry : fs::directory_iterator(folder_path)) {
        if (entry.path().extension() == ".xml") {
            pugi::xml_document doc;
            pugi::xml_parse_result result = doc.load_file(entry.path().c_str());

            if (result) {
                std::cout << "\n============================================\n";
                std::cout << "Archivo: " << entry.path().filename() << "\n";
                
                pugi::xml_node book_node = doc.child("GoodreadsResponse").child("book");
                if (book_node) {
                    Tree book_tree(100); // Create tree with high k capacity for XML
                    int id_counter = 0;
                    std::string root_id = "node_" + std::to_string(++id_counter);
                    
                    XmlNodeData root_data;
                    root_data.id = root_id;
                    root_data.tag = book_node.name();
                    
                    std::string root_text = book_node.child_value();
                    size_t start = root_text.find_first_not_of(" \t\n\r");
                    if (start != std::string::npos) {
                        size_t end = root_text.find_last_not_of(" \t\n\r");
                        root_data.text_content = root_text.substr(start, end - start + 1);
                    }
                    
                    book_tree.insert("", root_data); // insert root
                    
                    populate_tree(book_node, book_tree, root_id, id_counter);
                    
                    book_tree.printTree();
                    
                } else {
                    std::cerr << "Error: No se encontro el nodo <book> en el XML.\n";
                }
            } else {
                std::cerr << "Failed to parse " << entry.path().filename() 
                          << " - Error: " << result.description() << std::endl;
            }

            count++;
            // Limit to 5 files for the sample output
            if (count >= 5) {
                std::cout << "Stopping after 5 files for this sample demonstration." << std::endl;
                break;
            }
        }
    }

    if (count == 0) {
        std::cout << "No XML files found in the directory." << std::endl;
    }

    return 0;
}
