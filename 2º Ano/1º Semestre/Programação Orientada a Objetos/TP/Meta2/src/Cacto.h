#ifndef CACTO_H
#define CACTO_H

#include "Planta.h"
#include "Jardim.h"

class Cacto : public Planta {
    double total_nutrientes_absorvidos = 0.0;
    int instantes_agua_alta = 0;
    int instantes_nutrientes_zero = 0;
    int instantes_encharcado = 0;
    int instantes_desde_multiplicacao = 0;

public:
    Cacto();
    Cacto(const Cacto& outro);
    virtual ~Cacto() = default;

    char getRepresentacao() const override;

    bool estaVivo() const override;

    void passarInstante(Posicao &pos) override;

    void liberarRecursos(Posicao &pos) override;

    void listarPropriedades() const override;

    bool podeMultiplicar() const;

    Cacto* tentarMultiplicar(Jardim* j, int linha, int coluna);

    Planta* clone() const override;
};

#endif