#include "tree.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

// Funciones auxiliares para imprimir en colores y validar
const std::string GREEN = "\033[32m";
const std::string RED = "\033[31m";
const std::string RESET = "\033[0m";

void assert_test(const std::string& test_name, bool condition) {
    if (condition) {
        std::cout << "[ " << GREEN << "PASS" << RESET << " ] " << test_name << "\n";
    } else {
        std::cout << "[ " << RED << "FAIL" << RESET << " ] " << test_name << "\n";
    }
}

bool contains(const std::vector<int>& vec, int element) {
    return std::find(vec.begin(), vec.end(), element) != vec.end();
}

int main() {
    std::cout << "=========================================================\n";
    std::cout << "   TESTS PARA LAS FUNCIONES: listar, precursores, borrar_ratings\n";
    std::cout << "=========================================================\n\n";

    Tree tree(5000); // k grande para permitir muchos hijos
    
    // 1. Insertar Raíz Global (como en main.cpp)
    tree.insert(0, {1, "root_catalogo", ""});

    // ---------------------------------------------------------------------------------
    // CASO 1: Libro normal precursor (Año 2000, similares en 2005 y 2001)
    // Rating: 4.0
    // ---------------------------------------------------------------------------------
    tree.insert(1, {10, "book", ""});
    tree.insert(10, {11, "publication_year", "2000"});
    tree.insert(10, {12, "average_rating", "4.0"});
    tree.insert(10, {13, "similar_books", ""});
        tree.insert(13, {131, "book", ""}); // Similar 1
        tree.insert(131, {1311, "publication_year", "2005"});
        tree.insert(13, {132, "book", ""}); // Similar 2
        tree.insert(132, {1321, "publication_year", "2001"});

    // ---------------------------------------------------------------------------------
    // CASO 2: Libro NO precursor por empate (Año 2010, similar en 2010)
    // Rating: 3.5
    // ---------------------------------------------------------------------------------
    tree.insert(1, {20, "book", ""});
    tree.insert(20, {21, "publication_year", "2010"});
    tree.insert(20, {22, "average_rating", "3.5"});
    tree.insert(20, {23, "similar_books", ""});
        tree.insert(23, {231, "book", ""});
        tree.insert(231, {2311, "publication_year", "2010"}); // No es mayor estricto

    // ---------------------------------------------------------------------------------
    // CASO 3: Libro NO precursor por año anterior (Año 2010, similar en 2009)
    // Rating: 2.0 (DEBE SER BORRADO con r=3.0)
    // ---------------------------------------------------------------------------------
    tree.insert(1, {30, "book", ""});
    tree.insert(30, {31, "publication_year", "2010"});
    tree.insert(30, {32, "average_rating", "2.0"});
    tree.insert(30, {33, "similar_books", ""});
        tree.insert(33, {331, "book", ""});
        tree.insert(331, {3311, "publication_year", "2009"});

    // ---------------------------------------------------------------------------------
    // CASO 4: Libro NO precursor por múltiples similares (uno rompe la regla)
    // Rating: 4.5
    // ---------------------------------------------------------------------------------
    tree.insert(1, {40, "book", ""});
    tree.insert(40, {41, "publication_year", "2000"});
    tree.insert(40, {42, "average_rating", "4.5"});
    tree.insert(40, {43, "similar_books", ""});
        tree.insert(43, {431, "book", ""});
        tree.insert(431, {4311, "publication_year", "2005"});
        tree.insert(43, {432, "book", ""});
        tree.insert(432, {4321, "publication_year", "1999"}); // Rompe la regla (1999 < 2000)

    // ---------------------------------------------------------------------------------
    // CASO 5: Libro sin similares (Técnicamente ES precursor ya que "todos" cumplen vacuamente)
    // Rating: 1.5 (DEBE SER BORRADO con r=3.0)
    // ---------------------------------------------------------------------------------
    tree.insert(1, {50, "book", ""});
    tree.insert(50, {51, "publication_year", "2015"});
    tree.insert(50, {52, "average_rating", "1.5"});
    tree.insert(50, {53, "similar_books", ""}); // Vacío, sin nodos <book> dentro

    // ---------------------------------------------------------------------------------
    // CASO 6: Libro precursor con rating en el límite de r (Rating: 3.0)
    // DEBE SER BORRADO si ejecutamos borrar_ratings(3.0)
    // ---------------------------------------------------------------------------------
    tree.insert(1, {60, "book", ""});
    tree.insert(60, {61, "publication_year", "1980"});
    tree.insert(60, {62, "average_rating", "3.0"});
    tree.insert(60, {63, "similar_books", ""});
        tree.insert(63, {631, "book", ""});
        tree.insert(631, {6311, "publication_year", "1985"}); // Precursor válido

    // =================================================================================
    // EJECUCIÓN DE PRUEBAS
    // =================================================================================
    
    std::cout << "--- PRUEBA DE listar() ---\n";
    std::vector<int> listados = tree.listar();
    std::cout << "Total de nodos 'book' encontrados (principales + similares): " << listados.size() << "\n";
    
    // Deberían estar los libros principales: 10, 20, 30, 40, 50, 60
    // Y los similares: 131, 132, 231, 331, 431, 432, 631
    assert_test("listar() incluye Libro 10", contains(listados, 10));
    assert_test("listar() incluye Libro 20", contains(listados, 20));
    assert_test("listar() incluye Libro 30", contains(listados, 30));
    assert_test("listar() incluye Libro 40", contains(listados, 40));
    assert_test("listar() incluye Libro 50", contains(listados, 50));
    assert_test("listar() incluye Libro 60", contains(listados, 60));
    assert_test("listar() incluye similar 131", contains(listados, 131));
    std::cout << "\n";

    std::cout << "--- PRUEBA DE precursores() ---\n";
    std::vector<int> prec = tree.precursores();
    
    // Precursores esperados: 10, 50, 60
    // No precursores: 20, 30, 40
    assert_test("precursores() incluye Libro 10 (2000 -> 2005, 2001)", contains(prec, 10));
    assert_test("precursores() omite Libro 20 (2010 -> 2010, empate)", !contains(prec, 20));
    assert_test("precursores() omite Libro 30 (2010 -> 2009, menor)", !contains(prec, 30));
    assert_test("precursores() omite Libro 40 (2000 -> uno de 1999)", !contains(prec, 40));
    assert_test("precursores() incluye Libro 50 (Sin similares, trivialmente cierto)", contains(prec, 50));
    assert_test("precursores() incluye Libro 60 (1980 -> 1985)", contains(prec, 60));
    std::cout << "\n";

    std::cout << "--- PRUEBA DE borrar_ratings(3.0) ---\n";
    tree.borrar_ratings(3.0); // Debería borrar los que tengan rating <= 3.0
    
    std::vector<int> listados_despues = tree.listar();
    
    // Ratings: 10(4.0), 20(3.5), 30(2.0), 40(4.5), 50(1.5), 60(3.0)
    // Esperados post-borrado: 10, 20, 40
    // Borrados esperados: 30, 50, 60
    assert_test("Libro 10 mantenido (Rating 4.0 > 3.0)", contains(listados_despues, 10));
    assert_test("Libro 20 mantenido (Rating 3.5 > 3.0)", contains(listados_despues, 20));
    assert_test("Libro 40 mantenido (Rating 4.5 > 3.0)", contains(listados_despues, 40));
    
    assert_test("Libro 30 borrado (Rating 2.0 <= 3.0)", !contains(listados_despues, 30));
    assert_test("Libro 50 borrado (Rating 1.5 <= 3.0)", !contains(listados_despues, 50));
    assert_test("Libro 60 borrado (Rating 3.0 <= 3.0)", !contains(listados_despues, 60));

    std::cout << "\n=========================================================\n";
    std::cout << "   TESTS FINALIZADOS\n";
    std::cout << "=========================================================\n";

    return 0;
}
