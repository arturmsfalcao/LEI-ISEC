#include "TesouraPoda.h"
#include "Posicao.h"
#include "Planta.h"
#include <iostream>

TesouraPoda::TesouraPoda() {}

TesouraPoda::TesouraPoda(const TesouraPoda& outra) : Ferramenta(outra) {}

char TesouraPoda::getSimbolo() const { return 't'; }

void TesouraPoda::usar(Posicao &pos) {
    Planta* p = pos.getPlanta();
    if (p != nullptr && p->getBeleza() == "Feia") {
        p->liberarRecursos(pos);
        delete p;
        pos.setPlanta(nullptr);
        std::cout << "[INFO] Tesoura de Poda eliminou uma planta feia.\n";
    }
}

Ferramenta* TesouraPoda::clone() const {
    return new TesouraPoda(*this);
}

void TesouraPoda::imprimirDetalhes() const {
    std::cout << "Tesoura de Poda (Eterno - Nao se desgasta)";
}