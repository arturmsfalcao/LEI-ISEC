#ifndef PACOTEADUBO_H
#define PACOTEADUBO_H

#include "Ferramenta.h"
#include "Settings.h"
#include <iostream>

class PacoteAdubo : public Ferramenta {
    int quantidade;

public:
    PacoteAdubo();
    PacoteAdubo(const PacoteAdubo& outra);

    char getSimbolo() const override;

    void usar(Posicao &pos) override;

    bool estaGasta() const override {
        return quantidade <= 0;
    }

    Ferramenta* clone() const override;
    
    void imprimirDetalhes() const override;
};

#endif