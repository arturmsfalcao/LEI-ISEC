#ifndef ROSEIRA_H
#define ROSEIRA_H

#include "Planta.h"
#include "Jardim.h"

class Roseira : public Planta {
    double total_agua_absorvida = 0.0;
    double total_nutrientes_absorvidos = 0.0;

public:
    Roseira();
    Roseira(const Roseira& outra);

    char getRepresentacao() const override;

    bool estaVivo() const;

    std::string getBeleza() const override { return "Bonita"; }

    void passarInstante(Posicao &pos) override;

    bool deveMorrerPorSufocamento(Jardim* j, int linha, int coluna) const;

    void liberarRecursos(Posicao &pos) override;

    bool podeMultiplicar() const;
    Roseira* tentarMultiplicar(Jardim* j, int linha, int coluna);

    Planta* clone() const override;

    void listarPropriedades() const override;
};

#endif