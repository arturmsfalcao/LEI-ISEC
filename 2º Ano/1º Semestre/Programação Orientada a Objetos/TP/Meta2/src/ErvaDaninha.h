#ifndef ERVADANINHA_H
#define ERVADANINHA_H

#include "Planta.h"
#include "Jardim.h"

class ErvaDaninha : public Planta {
    int instantes_vida = 0;
    int instantes_ultima_multiplicacao = 0;

public:
    ErvaDaninha();
    virtual ~ErvaDaninha() = default;
    ErvaDaninha(const ErvaDaninha& outra);

    char getRepresentacao() const override;

    bool estaVivo() const override;

    std::string getBeleza() const override { return "Feia"; }

    void passarInstante(Posicao &pos) override;

    void liberarRecursos(Posicao &pos) override;

    void listarPropriedades() const override;

    bool podeMultiplicar() const;

    void resetMultiplicacao();

    Planta* tentarMultiplicar(Jardim* j, int linha, int coluna);

    Planta* clone() const override;
};

#endif