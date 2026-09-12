#include "FerramentaZ.h"
#include "Posicao.h"
#include "Planta.h"
#include "Jardim.h"
#include"Jardineiro.h"
#include <iostream>

FerramentaZ::FerramentaZ() {}

FerramentaZ::FerramentaZ(const FerramentaZ& outra) : Ferramenta(outra) {}

char FerramentaZ::getSimbolo() const { return 'z'; }

void FerramentaZ::usar(Posicao &pos) {}

int FerramentaZ::usar(Jardim &j, Jardineiro &jardineiro) {
    int contadorferramentas = 0;
    for (int l = 0; l < j.getLinhas(); ++l) {
        for (int c = 0; c < j.getColunas(); ++c) {
            Posicao& pos = j.getPosicao(l, c);
            if (pos.getFerramenta() != nullptr) {
                jardineiro.adicionarFerramenta(pos.getFerramenta());
                pos.setFerramenta(nullptr);
                contadorferramentas++;
            }
        }
    }
    return contadorferramentas;
}

void FerramentaZ::imprimirDetalhes() const {
    std::cout << "Iman (Ferramenta Z)";
}

Ferramenta* FerramentaZ::clone() const {
    return new FerramentaZ(*this);
}