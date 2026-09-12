#ifndef PLANTAEXOTICA_H
#define PLANTAEXOTICA_H

#include "Planta.h"
#include "Jardim.h"
#include "Posicao.h"
#include "Settings.h"
#include <iostream>

class PlantaExotica : public Planta {
public:
    PlantaExotica();
    PlantaExotica(const PlantaExotica& outra);
    
    char getRepresentacao() const override { return 'x'; } 
    
    bool estaVivo() const override;

    void passarInstante(Posicao &pos) override;

    void liberarRecursos(Posicao &pos) override {}

    Planta* clone() const override;

    void listarPropriedades() const override;
};

#endif