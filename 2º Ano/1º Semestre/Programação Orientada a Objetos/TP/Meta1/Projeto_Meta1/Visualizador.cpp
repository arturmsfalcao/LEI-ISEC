#include "Visualizador.h"
#include <iostream>

void Visualizador::mostrar(const std::vector<std::vector<Posicao>> &grade) {
    int linhas = grade.size();
    int colunas = grade.empty() ? 0 : grade[0].size();

    std::cout << "\n  ";
    for (int c = 0; c < colunas; ++c)
        std::cout << static_cast<char>('A' + c);
    std::cout << "\n";

    for (int l = 0; l < linhas; ++l) {
        std::cout << static_cast<char>('A' + l) << " ";
        for (int c = 0; c < colunas; ++c)
            std::cout << grade[l][c].getSimbolo();
        std::cout << "\n";
    }
}
