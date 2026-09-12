#ifndef TESOURAPODA_H
#define TESOURAPODA_H

#include "Ferramenta.h"
#include <iostream>

class TesouraPoda : public Ferramenta {
public:
    TesouraPoda();
    TesouraPoda(const TesouraPoda& outra);

    char getSimbolo() const override;

    bool estaGasta() const override {
        return false;
    }

    void usar(Posicao &pos) override;

    Ferramenta* clone() const override;
    
    void imprimirDetalhes() const override;
};

#endif