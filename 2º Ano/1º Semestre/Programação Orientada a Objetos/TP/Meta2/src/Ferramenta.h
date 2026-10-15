#ifndef FERRAMENTA_H
#define FERRAMENTA_H

#include "Planta.h"

class Ferramenta {
protected:
    int numeroSerie;
    static int contadorSerie;

public:
    Ferramenta();
    virtual ~Ferramenta() {}

    virtual char getSimbolo() const = 0;

    virtual void usar(Posicao &pos) = 0;

    virtual bool estaGasta() const = 0;

    int getNumeroSerie() const { return numeroSerie; }

    virtual Ferramenta* clone() const = 0;

    virtual void imprimirDetalhes() const = 0;
};

#endif