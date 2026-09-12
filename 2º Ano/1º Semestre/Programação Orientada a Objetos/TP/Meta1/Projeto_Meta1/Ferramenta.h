#ifndef FERRAMENTA_H
#define FERRAMENTA_H

#include "Posicao.h"

// --- Classe base Ferramenta ---
class Ferramenta {
    int numeroSerie;
    static int contadorSerie;
public:
    Ferramenta();
    virtual ~Ferramenta() {}

    virtual char getSimbolo() const = 0;
    virtual void usar(Posicao &pos) = 0;
};

// --- Regador ---
class Regador : public Ferramenta {
    int aguaDisponivel;
public:
    Regador();

    char getSimbolo() const override;
    void usar(Posicao &pos) override;
};

// --- Pacote de adubo ---
class PacoteAdubo : public Ferramenta {
    int quantidade;
public:
    PacoteAdubo();

    char getSimbolo() const override;
    void usar(Posicao &pos) override;
};

// --- Tesoura de poda ---
class TesouraPoda : public Ferramenta {
public:
    TesouraPoda();

    char getSimbolo() const override;
    void usar(Posicao &pos) override;
};

// --- FerramentaZ ---
class Pa : public Ferramenta {
public:
    Pa();

    char getSimbolo() const override;
    void usar(Posicao &pos) override;
};

#endif
