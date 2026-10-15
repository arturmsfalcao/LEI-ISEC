#ifndef FERRAMENTAZ_H
#define FERRAMENTAZ_H

#include "Ferramenta.h"
#include "Jardim.h"
#include"Jardineiro.h"
#include <iostream>

class FerramentaZ : public Ferramenta {
public:
    FerramentaZ();
    FerramentaZ(const FerramentaZ& outra);

    char getSimbolo() const override;

    bool estaGasta() const override {
        return false;
    }

    int usar(Jardim &j, Jardineiro &jardineiro);

    void usar(Posicao &pos) override;

    Ferramenta* clone() const override;

    void imprimirDetalhes() const override;
};

#endif
