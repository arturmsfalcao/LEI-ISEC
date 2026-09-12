#include "PacoteAdubo.h"
#include "Posicao.h"
#include "Settings.h"

PacoteAdubo::PacoteAdubo() : quantidade(Settings::Adubo::capacidade) {}

PacoteAdubo::PacoteAdubo(const PacoteAdubo& outra) : Ferramenta(outra), quantidade(outra.quantidade) {}

char PacoteAdubo::getSimbolo() const { return 'a'; }

void PacoteAdubo::usar(Posicao &pos) {
    if (quantidade >= Settings::Adubo::dose) {
        pos.adicionarNutrientes(Settings::Adubo::dose);
        quantidade -= Settings::Adubo::dose;
    }
}

Ferramenta* PacoteAdubo::clone() const {
    return new PacoteAdubo(*this);
}

void PacoteAdubo::imprimirDetalhes() const {
    std::cout << "Pacote de Adubo (Quantidade: " << quantidade << "/" << Settings::Adubo::capacidade << " unidades)";
}