#include <iostream>
#include <filesystem>
#include <string>
#include "pugixml/pugixml.hpp"

namespace fs = std::filesystem;

void process_file(const fs::path& input_path, const fs::path& output_path) {
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(input_path.string().c_str());
    if (!result) {
        std::cerr << "Error parsing: " << input_path << " - " << result.description() << std::endl;
        return;
    }

    pugi::xml_node root = doc.child("GoodreadsResponse").child("book");
    if (!root) {
        std::cerr << "No book node in: " << input_path << std::endl;
        return;
    }

    pugi::xml_document out_doc;
    pugi::xml_node out_root = out_doc.append_child("book");

    auto add_child = [&](const char* name, pugi::xml_node src_node) {
        if (src_node) {
            pugi::xml_node new_node = out_root.append_child(name);
            new_node.text().set(src_node.text().get());
        }
    };

    add_child("id", root.child("id"));
    add_child("title", root.child("title"));
    add_child("isbn", root.child("isbn"));
    add_child("publication_year", root.child("publication_year"));
    add_child("language_code", root.child("language_code"));
    add_child("description", root.child("description"));
    add_child("average_rating", root.child("average_rating"));
    add_child("num_pages", root.child("num_pages"));

    pugi::xml_node similar_books = root.child("similar_books");
    if (similar_books) {
        pugi::xml_node out_similar = out_root.append_child("similar_books");
        for (pugi::xml_node book = similar_books.child("book"); book; book = book.next_sibling("book")) {
            pugi::xml_node out_book = out_similar.append_child("book");
            if (book.child("title")) out_book.append_child("title").text().set(book.child("title").text().get());
            if (book.child("isbn")) out_book.append_child("isbn").text().set(book.child("isbn").text().get());
            if (book.child("publication_year")) out_book.append_child("publication_year").text().set(book.child("publication_year").text().get());
        }
    }

    out_doc.save_file(output_path.string().c_str());
}

int main() {
    fs::path input_dir = "books_xml";
    fs::path output_dir = "books_xml_filtered";

    if (!fs::exists(output_dir)) {
        fs::create_directory(output_dir);
    }

    if (!fs::exists(input_dir) || !fs::is_directory(input_dir)) {
        std::cerr << "Input directory does not exist or is not a directory." << std::endl;
        return 1;
    }

    for (const auto& entry : fs::directory_iterator(input_dir)) {
        if (entry.is_regular_file() && entry.path().extension() == ".xml") {
            fs::path out_path = output_dir / entry.path().filename();
            process_file(entry.path(), out_path);
        }
    }

    std::cout << "Procesamiento completado." << std::endl;
    return 0;
}
