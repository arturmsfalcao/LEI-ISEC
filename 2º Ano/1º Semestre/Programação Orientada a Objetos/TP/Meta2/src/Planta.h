#ifndef PLANTA_H
#define PLANTA_H

#include <string>

class Posicao;

class Planta {
protected:
    int agua;
    int nutrientes;

public:
    Planta() : agua(0), nutrientes(0) {}
    virtual ~Planta() = default;

    virtual void passarInstante(Posicao &pos) = 0;
    virtual char getRepresentacao() const = 0;

    virtual bool estaVivo() const = 0;

    virtual std::string getBeleza() const { return "Neutra"; }

    virtual void listarPropriedades() const = 0;

    virtual void liberarRecursos(Posicao &pos) = 0;

    virtual Planta* clone() const = 0;
};

#endif