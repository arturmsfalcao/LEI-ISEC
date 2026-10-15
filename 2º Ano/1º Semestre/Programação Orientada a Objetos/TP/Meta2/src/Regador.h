#ifndef REGADOR_H
#define REGADOR_H

#include "Ferramenta.h"
#include "Settings.h"
#include <iostream>

class Regador : public Ferramenta {
    int aguaDisponivel;

public:
    Regador();
    Regador(const Regador& outra);

    char getSimbolo() const override;

    bool estaGasta() const override {
        return aguaDisponivel <= 0;
    }

    void usar(Posicao &pos) override;

    Ferramenta* clone() const override;

    void imprimirDetalhes() const override;
};

#endif