#include "Posicao.h"
#include "Settings.h"
#include "Planta.h"
#include "Ferramenta.h"
#include <cstdlib>
#include <iostream>

Posicao::Posicao() {
    agua = Settings::Jardim::agua_min + (rand() % (Settings::Jardim::agua_max - Settings::Jardim::agua_min + 1));
    nutrientes = Settings::Jardim::nutrientes_min + (rand() % (Settings::Jardim::nutrientes_max - Settings::Jardim::nutrientes_min + 1));
}

char Posicao::getSimbolo() const {
    if (planta) {
        return planta->getRepresentacao();
    }

    if (ferramenta) {
        return ferramenta->getSimbolo();
    }

    return ' ';
}

void Posicao::setCoordenadas(int l, int c) {
    linha = l;
    coluna = c;
}

void Posicao::listarRecursos() const {
    std::cout << "\n--- RECURSOS DO SOLO ("
              << (char)('A' + linha) << (char)('A' + coluna)
              << ") ---\n";
    std::cout << "Agua: " << agua << " / " << Settings::Jardim::agua_max << "\n";
    std::cout << "Nutrientes: " << nutrientes << " / " << Settings::Jardim::nutrientes_max << "\n";
    std::cout << "--------------------------------\n";
}