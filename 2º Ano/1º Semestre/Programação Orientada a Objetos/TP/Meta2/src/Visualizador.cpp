#include "Visualizador.h"
#include "Jardim.h"
#include "Jardineiro.h"
#include <iostream>

void Visualizador::mostrar(const Jardim& jardim, const Jardineiro& jardineiro) {
    int linhas = jardim.getLinhas();
    int colunas = jardim.getColunas();

    std::cout << "\n  ";
    for (int c = 0; c < colunas; ++c) {
        std::cout << static_cast<char>('A' + c);
    }
    std::cout << "\n";

    for (int l = 0; l < linhas; ++l) {
        std::cout << static_cast<char>('A' + l) << " ";

        for (int c = 0; c < colunas; ++c) {
            char simbolo = ' ';

            if (jardineiro.estaDentro() && jardineiro.getLinha() == l && jardineiro.getColuna() == c) {
                simbolo = '*';
            } else {
                simbolo = jardim.getPosicao(l, c).getSimbolo();
            }

            std::cout << simbolo;
        }
        std::cout << " " << static_cast<char>('A' + l);
        std::cout << "\n";
    }
    std::cout << "  ";
    for (int c = 0; c < colunas; ++c) {
        std::cout << static_cast<char>('A' + c);
    }
    std::cout << "\n";
}