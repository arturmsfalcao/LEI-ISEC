#include "Simulador.h"
#include <iostream>

void Simulador::criar(int l, int c) {
    if (jardimCriado) {
        std::cout << "[ERRO] O jardim ja foi criado.\n";
        return;
    }

    jardim = Jardim(l, c);
    jardimCriado = true;

    std::cout << "Jardim criado com " << l << " linhas e " << c << " colunas.\n";
    jardim.mostrar();
}
