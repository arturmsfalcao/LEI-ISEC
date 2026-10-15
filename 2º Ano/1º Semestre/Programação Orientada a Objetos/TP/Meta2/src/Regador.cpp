#include "Regador.h"
#include "Posicao.h"
#include "Settings.h"

Regador::Regador() : aguaDisponivel(Settings::Regador::capacidade) {}

Regador::Regador(const Regador& outra) : Ferramenta(outra), aguaDisponivel(outra.aguaDisponivel) {}

char Regador::getSimbolo() const { return 'g'; }

void Regador::usar(Posicao &pos) {
    if (aguaDisponivel >= Settings::Regador::dose) {
        pos.adicionarAgua(Settings::Regador::dose);
        aguaDisponivel -= Settings::Regador::dose;
    }
}

Ferramenta* Regador::clone() const {
    return new Regador(*this);
}

void Regador::imprimirDetalhes() const {
    std::cout << "Regador (Agua: " << aguaDisponivel << "/" << Settings::Regador::capacidade << " unidades)";
}