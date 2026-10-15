#ifndef PLANTA_H
#define PLANTA_H

#include "Posicao.h"

// --- Classe Planta ---
class Planta {
    int agua;
    int nutrientes;
    int tempoVivo;
public:
    Planta();
    virtual ~Planta();

    virtual char getSimbolo() const = 0;
    virtual void passarInstante(Posicao &pos) = 0;
    virtual bool morreu() const = 0;
};

// --- Classe Cacto ---
class Cacto : public Planta {
    int instantesAguaAlta;
    int instantesSemNutrientes;
public:
    Cacto();

    char getSimbolo() const override;
    void passarInstante(Posicao &pos) override;
    bool morreu() const override;
};

// --- Classe Roseira ---
class Roseira : public Planta {
public:
    Roseira();

    char getSimbolo() const override;
    void passarInstante(Posicao &pos) override;
    bool morreu() const override;
};

// --- Classe ErvaDaninha ---
class ErvaDaninha : public Planta {
    int contadorInstantes;
    int tempoUltimaMultiplicacao;
public:
    ErvaDaninha();

    char getSimbolo() const override;
    void passarInstante(Posicao &pos) override;
    bool morreu() const override;
};

// --- Classe PlantaExotica ---
class AgataAzul : public Planta {
public:
    AgataAzul();

    char getSimbolo() const override;
    void passarInstante(Posicao &pos) override;
    bool morreu() const override;
};

#endif
