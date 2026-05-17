#include <iostream>
#include <filesystem>
#include "pugixml/pugixml.hpp"

namespace fs = std::filesystem;

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
                    std::cout << "ID: " << book_node.child_value("id") << "\n";
                    std::cout << "Titulo: " << book_node.child_value("title") << "\n";
                    std::cout << "ISBN: " << book_node.child_value("isbn") << "\n";
                    std::cout << "Anio de publicacion: " << book_node.child_value("publication_year") << "\n";
                    std::cout << "Idioma: " << book_node.child_value("language_code") << "\n";
                    std::cout << "Descripcion: " << book_node.child_value("description") << "\n";
                    std::cout << "Rating promedio: " << book_node.child_value("average_rating") << "\n";
                    std::cout << "Numero de paginas: " << book_node.child_value("num_pages") << "\n";
                    
                    std::cout << "Libros similares:\n";
                    pugi::xml_node similar_books = book_node.child("similar_books");
                    for (pugi::xml_node similar = similar_books.child("book"); similar; similar = similar.next_sibling("book")) {
                        std::cout << "  - Titulo: " << similar.child_value("title") << "\n";
                        std::cout << "    ISBN: " << similar.child_value("isbn") << "\n";
                        std::cout << "    Anio de publicacion: " << similar.child_value("publication_year") << "\n";
                    }
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
